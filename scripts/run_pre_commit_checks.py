import os
import sys
import subprocess
import time
import urllib.request
import shutil
import platform
import tarfile
import zipfile
import atexit

def run_cmd(cmd, cwd=None, env=None, capture_output=False, check=True):
    print(f"Running: {cmd if isinstance(cmd, str) else ' '.join(cmd)}")
    
    if env is None:
        env = os.environ.copy()
    for key in ['GIT_DIR', 'GIT_INDEX_FILE', 'GIT_WORK_TREE']:
        env.pop(key, None)
        
    result = subprocess.run(cmd, cwd=cwd, env=env, shell=isinstance(cmd, str), capture_output=capture_output, text=True)
    if check and result.returncode != 0:
        print(f"Command failed with exit code {result.returncode}")
        if capture_output:
            print(f"Stdout:\n{result.stdout}")
            print(f"Stderr:\n{result.stderr}")
        sys.exit(result.returncode)
    return result

def build_project():
    print("Building project...")
    run_cmd(["cmake", "-B", "build", "-S", ".", "-DCMAKE_BUILD_TYPE=Release"])
    run_cmd(["cmake", "--build", "build", "--config", "Release"])

def run_tests():
    print("Running tests...")
    test_bin = None
    if sys.platform == "win32":
        test_bin_rel = os.path.join("build", "Release", "cdd-tests.exe")
        test_bin_root = os.path.join("build", "cdd-tests.exe")
        if os.path.exists(test_bin_rel):
            test_bin = test_bin_rel
        else:
            test_bin = test_bin_root
    else:
        test_bin = os.path.join("build", "cdd-tests")
    run_cmd([test_bin])

def build_wasm():
    print("Building WASM via wasi-sdk...")
    wasi_dir = "wasi-sdk"
    if not os.path.exists(wasi_dir):
        os_name = platform.system().lower()
        arch = platform.machine().lower()
        if os_name == "darwin":
            wasi_os = "macos"
        elif os_name == "windows":
            wasi_os = "windows"
        else:
            wasi_os = "linux"
            
        if arch in ["x86_64", "amd64"]:
            wasi_arch = "x86_64"
        elif arch in ["arm64", "aarch64"]:
            wasi_arch = "arm64"
        else:
            wasi_arch = "x86_64"
            
        url = f"https://github.com/WebAssembly/wasi-sdk/releases/download/wasi-sdk-24/wasi-sdk-24.0-{wasi_arch}-{wasi_os}.tar.gz"
        tar_path = f"wasi-sdk-24.0-{wasi_arch}-{wasi_os}.tar.gz"
        print(f"Downloading {url}...")
        urllib.request.urlretrieve(url, tar_path)
        
        print("Extracting wasi-sdk...")
        with tarfile.open(tar_path) as tar:
            tar.extractall()
        os.remove(tar_path)
        
        for d in os.listdir("."):
            if d.startswith("wasi-sdk-24.0") and os.path.isdir(d):
                os.rename(d, wasi_dir)
                break
                
    cmake_path = os.path.join(wasi_dir, "share", "cmake", "wasi-sdk.cmake")
    with open(cmake_path, "r") as f:
        content = f.read()
    content = content.replace("VERSION 3.4.0", "VERSION 3.11")
    with open(cmake_path, "w") as f:
        f.write(content)
        
    build_dir = "build_wasm"
    if os.path.exists(build_dir):
        shutil.rmtree(build_dir)
    os.makedirs(build_dir)
    
    toolchain_file = os.path.abspath(os.path.join(wasi_dir, "share", "cmake", "wasi-sdk.cmake"))
    cmake_args = [
        "cmake", "..",
        f"-DCMAKE_TOOLCHAIN_FILE={toolchain_file}",
        "-DCDD_EXTREME_CHECKS=OFF",
        "-DSIMDJSON_ENABLE_THREADS=OFF",
        "-DCMAKE_BUILD_TYPE=Release",
        "-DCMAKE_CXX_FLAGS=-fno-exceptions -DSIMDJSON_EXCEPTIONS=1",
        "-DCMAKE_C_FLAGS=-fno-exceptions"
    ]
    if sys.platform == "win32":
        cmake_args.extend(["-G", "MinGW Makefiles"])
    run_cmd(cmake_args, cwd=build_dir)
    
    run_cmd([sys.executable, "../patch_simdjson_simple.py"], cwd=build_dir)
    
    run_cmd(["cmake", "--build", ".", "--target", "cdd-cpp"], cwd=build_dir)
    
    if not os.path.exists("bin"):
        os.makedirs("bin")
    
    wasm_bin = os.path.join(build_dir, "cdd-cpp")
    if sys.platform == "win32" and os.path.exists(wasm_bin + ".exe"):
        wasm_bin += ".exe"
    shutil.copy2(wasm_bin, os.path.join("bin", "cdd-cpp.wasm"))


def start_local_petstore(base_path, port=8080):
    v2_dir = os.path.abspath(os.path.join("..", "swagger-petstore-v2"))
    jetty_jar = os.path.join(v2_dir, "target", "lib", "jetty-runner.jar")
    
    if not os.path.exists(jetty_jar):
        print("Building local swagger-petstore-v2...")
        run_cmd(["mvn", "package", "-DskipTests"], cwd=v2_dir)

    war_path = None
    for f in os.listdir(os.path.join(v2_dir, "target")):
        if f.startswith("swagger-petstore-v2-") and f.endswith(".war"):
            war_path = os.path.join(v2_dir, "target", f)
            break
            
    if not war_path:
        raise Exception("Could not find swagger-petstore-v2 war file.")
        
    webapp_dir = os.path.abspath(os.path.join("build", f"petstore_webapp_{port}"))
    if os.path.exists(webapp_dir):
        shutil.rmtree(webapp_dir)
    os.makedirs(webapp_dir)
    
    with zipfile.ZipFile(war_path, 'r') as zip_ref:
        zip_ref.extractall(webapp_dir)
        
    web_xml = os.path.join(webapp_dir, "WEB-INF", "web.xml")
    with open(web_xml, "r") as f:
        xml_content = f.read()
        
    url = f"http://127.0.0.1:{port}"
    full_path = f"{url}{base_path}"
    
    xml_content = xml_content.replace("SWAGGER_HOST", url)
    xml_content = xml_content.replace("BASE_PATH", base_path)
    xml_content = xml_content.replace("FULL_SWAGGER_PATH", full_path)
    
    with open(web_xml, "w") as f:
        f.write(xml_content)
        
    index_html = os.path.join(webapp_dir, "index.html")
    if os.path.exists(index_html):
        with open(index_html, "r") as f:
            idx_content = f.read()
        idx_content = idx_content.replace("SWAGGER_HOST", url)
        idx_content = idx_content.replace("FULL_SWAGGER_PATH", full_path)
        with open(index_html, "w") as f:
            f.write(idx_content)
            
    print(f"Starting local petstore server on port {port} with base path {base_path}...")
    log_file = open(os.path.abspath(os.path.join("build", f"petstore_{port}.log")), "w")
    proc = subprocess.Popen(["java", "-jar", jetty_jar, "--port", str(port), webapp_dir], stdout=log_file, stderr=subprocess.STDOUT)
    return proc

def cleanup_petstore(proc):
    if proc:
        proc.terminate()
        try:
            proc.wait(timeout=5)
        except subprocess.TimeoutExpired:
            proc.kill()

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

def main():
    try:
        build_project()
        
        run_tests()
        
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
        
        petstore_proc = start_local_petstore("/v2", 8081)
        try:
            if wait_for_url("http://127.0.0.1:8081/v2/swagger.json"):
                v2_env = os.environ.copy()
                v2_env["PETSTORE_URL"] = "http://127.0.0.1:8081/v2"
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
                print("Skipping Swagger 2.0 tests since Java server did not start in time.")
        finally:
            cleanup_petstore(petstore_proc)
            
        # OpenAPI 3.2.0 Petstore test
        print("Running OpenAPI 3.2.0 Petstore test...")
        v3_out_dir = os.path.abspath(os.path.join("..", "cdd-cpp-client-v3"))
        if os.path.exists(v3_out_dir):
            shutil.rmtree(v3_out_dir)
            
        petstore_oas3_json = os.path.abspath(os.path.join("..", "petstore_oas3.json"))
        run_cmd([cdd_cpp_bin, "from_openapi", "to_sdk", "-i", petstore_oas3_json, "-o", v3_out_dir, "--tests"])
        
        petstore_proc = start_local_petstore("/api/v3", 8082)
        try:
            if wait_for_url("http://127.0.0.1:8082/api/v3/swagger.json"):
                v3_env = os.environ.copy()
                v3_env["PETSTORE_URL"] = "http://127.0.0.1:8082/api/v3"
                run_cmd(["cmake", "."], cwd=v3_out_dir)
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
                print("Skipping OpenAPI 3.2.0 tests since Java server did not start in time.")
        finally:
            cleanup_petstore(petstore_proc)
            
        build_wasm()
        
        print("Pre-commit checks passed.")
    except Exception as e:
        print(f"Error during pre-commit checks: {e}")
        
        sys.exit(1)

if __name__ == "__main__":
    main()
