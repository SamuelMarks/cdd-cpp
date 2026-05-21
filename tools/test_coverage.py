import subprocess
import re
import os

def get_coverage():
    try:
        # Check if tests exist
        if not os.path.exists("build/cdd-tests"):
            subprocess.run(
                ["cmake", "-B", "build", "-S", ".", "-DCMAKE_BUILD_TYPE=Release"],
                check=True, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL
            )
            subprocess.run(
                ["cmake", "--build", "build", "-j4"],
                check=True, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL
            )
            
        # Run tests and parse simulated coverage
        result = subprocess.run(
            ["./cdd-tests"],
            cwd="build",
            capture_output=True, text=True, check=True
        )
        
        match = re.search(r"(\d+(?:\.\d+)?)%\s+simulated coverage", result.stdout)
        if match:
            return f"{float(match.group(1)):.2f}"
            
    except Exception as e:
        print(f"Error calculating coverage: {e}")

    return "0.00"

if __name__ == "__main__":
    print(get_coverage())
