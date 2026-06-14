import urllib.request
import json
import subprocess
import os

def run_test():
    url = "https://discovery.googleapis.com/discovery/v1/apis/discovery/v1/rest"
    json_path = "discovery_v1.json"
    
    print(f"Downloading {url}...")
    urllib.request.urlretrieve(url, json_path)
    
    output_dir = "generated_discovery_sdk"
    print(f"Generating SDK in {output_dir}...")
    
    # Generate SDK
    subprocess.run([os.path.join(os.path.dirname(os.path.dirname(__file__)), "build", "cdd-cpp"), "from_google_discovery", "to_sdk", "-i", json_path, "-o", output_dir, "--tests"], check=True)
    
    print("Building generated SDK...")
    build_dir = os.path.join(output_dir, "build")
    os.makedirs(build_dir, exist_ok=True)
    
    subprocess.run(["cmake", "-S", output_dir, "-B", build_dir], check=True)
    subprocess.run(["cmake", "--build", build_dir], check=True)
    
    print("Running generated tests...")
    subprocess.run(["ctest", "--test-dir", build_dir, "--output-on-failure"], check=True)
    
    print("Generating CLI...")
    cli_output_dir = "generated_discovery_cli"
    subprocess.run([os.path.join(os.path.dirname(os.path.dirname(__file__)), "build", "cdd-cpp"), "from_google_discovery", "to_sdk_cli", "-i", json_path, "-o", cli_output_dir, "--tests"], check=True)
    
    cli_build_dir = os.path.join(cli_output_dir, "build")
    os.makedirs(cli_build_dir, exist_ok=True)
    
    print("Building generated CLI...")
    subprocess.run(["cmake", "-S", cli_output_dir, "-B", cli_build_dir], check=True)
    subprocess.run(["cmake", "--build", cli_build_dir], check=True)
    
    print("Success!")

if __name__ == "__main__":
    run_test()
