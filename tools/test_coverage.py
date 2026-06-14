import subprocess
import re
import os
import sys

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
            
        # Run tests and parse simulated coverage - wait, no, I want actual gcovr output!
        # The previous "true" gcovr was:
        result = subprocess.run(
            ["gcovr", "--print-summary", "-r", ".", "--filter", "src/", "--gcov-ignore-parse-errors=all"],
            capture_output=True, text=True, check=True
        )
        
        # Parse output for lines coverage
        match = re.search(r"lines:\s+([\d\.]+)%", result.stdout)
        if match:
            return f"{float(match.group(1)):.2f}"
            
    except Exception as e:
        print(f"Error calculating coverage: {e}")

    return "0.00"

if __name__ == "__main__":
    cov = get_coverage()
    print(cov)
    if float(cov) < 95.0:
        sys.exit(1)
