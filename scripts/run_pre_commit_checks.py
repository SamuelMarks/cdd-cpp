import os
import sys
import subprocess
import time
import urllib.request
import shutil

def run_cmd(cmd, cwd=None, env=None, capture_output=False, check=True):
    print(f"Running: {cmd if isinstance(cmd, str) else ' '.join(cmd)}")
    result = subprocess.run(cmd, cwd=cwd, env=env, shell=isinstance(cmd, str), capture_output=capture_output, text=True)
    if check and result.returncode != 0:
        print(f"Command failed with exit code {result.returncode}")
        if capture_output:
            print(f"Stdout:\n{result.stdout}")
            print(f"Stderr:\n{result.stderr}")
        sys.exit(result.returncode)
    return result

def run_make(target):
    if sys.platform == "win32":
        run_cmd(f"make.bat {target}")
    else:
        run_cmd(["make", target])

def docker_available():
    try:
        res = subprocess.run(["docker", "info"], capture_output=True)
        return res.returncode == 0
    except FileNotFoundError:
        return False

def wait_for_url(url, timeout=150):
    start_time = time.time()
    print(f"Waiting for {url} to be available...")
    while time.time() - start_time < timeout:
        try:
            req = urllib.request.Request(url)
            with urllib.request.urlopen(req, timeout=5) as response:
                if response.status == 200:
                    print(f"URL {url} is available!")
                    return True
        except Exception as e:
            pass
        time.sleep(5)
    print(f"Timeout waiting for {url}")
    return False

def cleanup_docker():
    if docker_available():
        subprocess.run(["docker", "rm", "-f", "petstore_server_v2"], capture_output=True)
        subprocess.run(["docker", "rm", "-f", "petstore_server_v3"], capture_output=True)

def main():
    try:
        print("Building project...")
        run_make("build")
        
        print("Running tests...")
        run_make("test")
        
        print("Calculating coverage...")
        test_cov_res = run_cmd([sys.executable, "tools/test_coverage.py"], capture_output=True, check=False)
        test_cov = test_cov_res.stdout.strip() if test_cov_res.returncode == 0 else "0"
        
        doc_cov_res = run_cmd([sys.executable, "tools/doc_coverage.py", "src"], capture_output=True, check=False)
        doc_cov = doc_cov_res.stdout.strip() if doc_cov_res.returncode == 0 else "0"
        
        print(f"Coverage: Test={test_cov}, Doc={doc_cov}")
        
        print("Updating README badges...")
        run_cmd([sys.executable, "tools/update_badges.py", test_cov, doc_cov])
        
        # Add README to commit if changed
        diff_res = subprocess.run(["git", "diff", "--quiet", "README.md"])
        if diff_res.returncode != 0:
            print("Staging updated README.md")
            run_cmd(["git", "add", "README.md"])
            
        cdd_cpp_bin = None
        if sys.platform == "win32":
            if os.path.exists(os.path.join("build", "Release", "cdd-cpp.exe")):
                cdd_cpp_bin = os.path.abspath(os.path.join("build", "Release", "cdd-cpp.exe"))
            else:
                cdd_cpp_bin = os.path.abspath(os.path.join("build", "cdd-cpp.exe"))
        else:
            cdd_cpp_bin = os.path.abspath(os.path.join("build", "cdd-cpp"))
            
        # Swagger 2.0 Petstore test
        print("Running Swagger 2.0 Petstore test...")
        v2_out_dir = os.path.abspath(os.path.join("..", "cdd-cpp-client-v2"))
        if os.path.exists(v2_out_dir):
            shutil.rmtree(v2_out_dir)
        
        petstore_json = os.path.abspath(os.path.join("..", "petstore.json"))
        run_cmd([cdd_cpp_bin, "from_openapi", "to_sdk", "-i", petstore_json, "-o", v2_out_dir, "--tests"])
        
        if docker_available():
            subprocess.run(["docker", "rm", "-f", "petstore_server_v2"], capture_output=True)
            run_cmd("docker run -d -p 8080:8080 -e SWAGGER_HOST=\"http://localhost:8080\" -e SWAGGER_BASE_PATH=\"/v2\" --name petstore_server_v2 swaggerapi/petstore")
            if wait_for_url("http://localhost:8080/v2/swagger.json"):
                v2_env = os.environ.copy()
                v2_env["PETSTORE_URL"] = "http://localhost:8080/v2"
                run_cmd(["cmake", ".", "-DFETCHCONTENT_UPDATES_DISCONNECTED=ON"], cwd=v2_out_dir)
                run_cmd(["cmake", "--build", "."], cwd=v2_out_dir)
                client_test_bin = None
                if sys.platform == "win32":
                    if os.path.exists(os.path.join(v2_out_dir, "tests", "Release", "client_test.exe")):
                        client_test_bin = os.path.join(v2_out_dir, "tests", "Release", "client_test.exe")
                    else:
                        client_test_bin = os.path.join(v2_out_dir, "tests", "client_test.exe")
                else:
                    client_test_bin = os.path.join(v2_out_dir, "tests", "client_test")
                run_cmd([client_test_bin], cwd=v2_out_dir, env=v2_env)
            else:
                print("Skipping Swagger 2.0 tests since Docker server did not start in time.")
            subprocess.run(["docker", "rm", "-f", "petstore_server_v2"], capture_output=True)
        else:
            print("Skipping Swagger 2.0 tests since Docker is not available.")
            
        # OpenAPI 3.2.0 Petstore test
        print("Running OpenAPI 3.2.0 Petstore test...")
        v3_out_dir = os.path.abspath(os.path.join("..", "cdd-cpp-client-v3"))
        if os.path.exists(v3_out_dir):
            shutil.rmtree(v3_out_dir)
            
        petstore_oas3_json = os.path.abspath(os.path.join("..", "petstore_oas3.json"))
        run_cmd([cdd_cpp_bin, "from_openapi", "to_sdk", "-i", petstore_oas3_json, "-o", v3_out_dir, "--tests"])
        
        if docker_available():
            subprocess.run(["docker", "rm", "-f", "petstore_server_v3"], capture_output=True)
            run_cmd("docker run -d -p 8080:8080 -e SWAGGER_HOST=\"http://localhost:8080\" -e SWAGGER_BASE_PATH=\"/api/v3\" --name petstore_server_v3 swaggerapi/petstore")
            if wait_for_url("http://localhost:8080/api/v3/openapi.json"):
                v3_env = os.environ.copy()
                v3_env["PETSTORE_URL"] = "http://localhost:8080/api/v3"
                run_cmd(["cmake", ".", "-DFETCHCONTENT_UPDATES_DISCONNECTED=ON"], cwd=v3_out_dir)
                run_cmd(["cmake", "--build", "."], cwd=v3_out_dir)
                client_test_bin = None
                if sys.platform == "win32":
                    if os.path.exists(os.path.join(v3_out_dir, "tests", "Release", "client_test.exe")):
                        client_test_bin = os.path.join(v3_out_dir, "tests", "Release", "client_test.exe")
                    else:
                        client_test_bin = os.path.join(v3_out_dir, "tests", "client_test.exe")
                else:
                    client_test_bin = os.path.join(v3_out_dir, "tests", "client_test")
                run_cmd([client_test_bin], cwd=v3_out_dir, env=v3_env)
            else:
                print("Skipping OpenAPI 3.2.0 tests since Docker server did not start in time.")
            subprocess.run(["docker", "rm", "-f", "petstore_server_v3"], capture_output=True)
        else:
            print("Skipping OpenAPI 3.2.0 tests since Docker is not available.")
            
        print("Building WASM...")
        run_make("build_wasm")
        
        print("Pre-commit checks passed.")
    except Exception as e:
        print(f"Error during pre-commit checks: {e}")
        cleanup_docker()
        sys.exit(1)

if __name__ == "__main__":
    main()
