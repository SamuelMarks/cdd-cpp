import subprocess
import re
import os

def get_coverage():
    try:
        # Check if coverage data exists
        if not os.path.exists("build/CMakeFiles/cdd_lib.dir/src"):
            # Try to build with coverage if not present
            subprocess.run(
                ["cmake", "-B", "build", "-S", ".", "-DCMAKE_BUILD_TYPE=Debug", "-DCOVERAGE=ON"],
                check=True, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL
            )
            subprocess.run(
                ["cmake", "--build", "build", "-j4"],
                check=True, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL
            )
            subprocess.run(
                ["ctest"], cwd="build",
                check=True, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL
            )
            
        # Extract coverage summary
        result = subprocess.run(
            ["gcovr", "--print-summary", "-r", ".", "--filter", "src/", "--gcov-ignore-parse-errors=all"],
            capture_output=True, text=True, check=True
        )
        
        # Parse output for lines coverage
        match = re.search(r"lines:\s+([\d\.]+)%", result.stdout)
        if match:
            return f"{float(match.group(1)):.2f}"
            
    except subprocess.CalledProcessError as e:
        print(f"Error calculating coverage: {e}")
    except Exception as e:
        print(f"Error: {e}")

    return "0.00"

if __name__ == "__main__":
    print(get_coverage())
