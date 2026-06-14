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
    print(f"Running: {cmd if isinstance(cmd, str) else ' '.join(cmd)}", flush=True)
    
    if env is None:
        env = os.environ.copy()
    for key in ['GIT_DIR', 'GIT_INDEX_FILE', 'GIT_WORK_TREE']:
        env.pop(key, None)
        
    if not capture_output:
        result = subprocess.run(cmd, cwd=cwd, env=env, shell=isinstance(cmd, str))
    else:
        result = subprocess.run(cmd, cwd=cwd, env=env, shell=isinstance(cmd, str), capture_output=True, text=True)
        
    if check and result.returncode != 0:
        print(f"Command failed with exit code {result.returncode}", flush=True)
        if capture_output:
            print(result.stdout, flush=True)
            print(result.stderr, flush=True)
        import sys
        sys.exit(result.returncode)
    return result

def build_project():
    print("Building project...")
    run_cmd(["cmake", "-B", "build", "-S", ".", "-DCMAKE_BUILD_TYPE=Release", "-DCOVERAGE=ON"])
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
    
    # Patch simdjson to replace throw with abort
    patch_script = os.path.join(build_dir, "patch_simdjson.py")
    with open(patch_script, "w") as f:
        f.write("""
import os
for root, _, files in os.walk("_deps"):
    for file in files:
        if file.endswith(".h") or file.endswith(".cpp") or file.endswith("-inl.h"):
            path = os.path.join(root, file)
            try:
                with open(path, "r", encoding="utf-8") as f:
                    content = f.read()
                if "throw " in content:
                    import re
                    content = re.sub(r"throw\s+simdjson_error\(.*?\);", "abort();", content)
                    content = re.sub(r"throw\s+simdjson::simdjson_error\(.*?\);", "abort();", content)
                    content = content.replace("throw std::runtime_error", "// throw std::runtime_error")
                    content = content.replace("throw std::out_of_range", "// throw std::out_of_range")
                    with open(path, "w", encoding="utf-8") as f:
                        f.write(content)
            except Exception:
                pass
""")
    run_cmd([sys.executable, "patch_simdjson.py"], cwd=build_dir)
    
    run_cmd(["cmake", "--build", ".", "--target", "cdd-cpp"], cwd=build_dir)
    
    if not os.path.exists("bin"):
        os.makedirs("bin")
    
    wasm_bin = os.path.join(build_dir, "cdd-cpp")
    if sys.platform == "win32" and os.path.exists(wasm_bin + ".exe"):
        wasm_bin += ".exe"
    shutil.copy2(wasm_bin, os.path.join("bin", "cdd-cpp.wasm"))



def is_docker_running():
    try:
        subprocess.run(["docker", "info"], check=True, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
        return True
    except (subprocess.CalledProcessError, FileNotFoundError):
        return False

def run_docker_petstore(base_path, port):
    container_name = f"petstore_server_{port}"
    subprocess.run(["docker", "rm", "-f", container_name], stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
    cmd = [
        "docker", "run", "-d", "-p", f"{port}:8080",
        "-e", f"SWAGGER_HOST=http://127.0.0.1:{port}",
        "-e", f"SWAGGER_BASE_PATH={base_path}",
        "--name", container_name,
        "swaggerapi/petstore"
    ]
    run_cmd(cmd)
    return {"type": "docker", "name": container_name}

def is_java_available():
    try:
        import subprocess
        subprocess.run(["java", "-version"], stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL, check=True)
        return True
    except (subprocess.CalledProcessError, FileNotFoundError):
        return False

def start_local_petstore(base_path, port=8080):
    if is_java_available():
        print(f"JVM found locally. Starting native JVM petstore for port {port}...")
        v2_dir = os.path.abspath(os.path.join("..", "swagger-petstore-v2"))
        jetty_jar = os.path.join(v2_dir, "target", "lib", "jetty-runner.jar")
        
        if not os.path.exists(v2_dir):
            print(f"Cloning swagger-petstore-v2 to {v2_dir}...")
            run_cmd(["git", "clone", "https://github.com/swagger-api/swagger-petstore.git", v2_dir])
            
        if not os.path.exists(jetty_jar):
            print("Building local swagger-petstore-v2...")
            run_cmd(["mvn", "package", "-DskipTests"], cwd=v2_dir)

        war_path = None
        for f in os.listdir(os.path.join(v2_dir, "target")):
            if f.startswith("swagger-petstore-") and f.endswith(".war"):
                war_path = os.path.join(v2_dir, "target", f)
                break
                
        if not war_path:
            raise Exception("Could not find swagger-petstore war file.")
            
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
        return {"type": "jvm", "proc": proc}
    elif is_docker_running():
        print(f"JVM not found. Starting petstore with Docker on port {port}...")
        return run_docker_petstore(base_path, port)
    else:
        print("Error: Neither JVM nor Docker is available to start the petstore server.")
        import sys
        sys.exit(1)


def cleanup_petstore(proc_info):
    if not proc_info:
        return
    if isinstance(proc_info, dict) and proc_info.get("type") == "docker":
        print(f"Stopping docker container {proc_info['name']}...")
        subprocess.run(["docker", "rm", "-f", proc_info["name"]], stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
    else:
        proc = proc_info.get("proc") if isinstance(proc_info, dict) else proc_info
        if proc:
            print("Stopping JVM petstore...")
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
        
        badges_updated = False
        diff_res = subprocess.run(["git", "diff", "--quiet", "README.md"])
        if diff_res.returncode != 0:
            print("README.md was updated with new badges.")
            badges_updated = True
            
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
        
        petstore_proc = start_local_petstore("/v2", 8084)
        try:
            if wait_for_url("http://127.0.0.1:8084/v2/swagger.json"):
                v2_env = os.environ.copy()
                v2_env["PETSTORE_URL"] = "http://127.0.0.1:8084/v2"
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
                raise Exception("Swagger 2.0 tests failed because Java server did not start in time.")
        finally:
            cleanup_petstore(petstore_proc)
            
        # OpenAPI 3.2.0 Petstore test
        print("Running OpenAPI 3.2.0 Petstore test...")
        v3_out_dir = os.path.abspath(os.path.join("..", "cdd-cpp-client-v3"))
        if os.path.exists(v3_out_dir):
            shutil.rmtree(v3_out_dir)
            
        petstore_oas3_json = os.path.abspath(os.path.join("..", "petstore_oas3.json"))
        run_cmd([cdd_cpp_bin, "from_openapi", "to_sdk", "-i", petstore_oas3_json, "-o", v3_out_dir, "--tests"])
        
        petstore_proc = start_local_petstore("/api/v3", 8085)
        try:
            if wait_for_url("http://127.0.0.1:8085/api/v3/swagger.json"):
                v3_env = os.environ.copy()
                v3_env["PETSTORE_URL"] = "http://127.0.0.1:8085/api/v3"
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
                raise Exception("OpenAPI 3.2.0 tests failed because Java server did not start in time.")
        finally:
            cleanup_petstore(petstore_proc)
            
        build_wasm()
        
        print("Pre-commit checks passed.")
        if badges_updated:
            print("README.md was updated with new badges. Please stage it and commit again.")
            sys.exit(1)
    except Exception as e:
        print(f"Error during pre-commit checks: {e}")
        
        sys.exit(1)

if __name__ == "__main__":
    main()
