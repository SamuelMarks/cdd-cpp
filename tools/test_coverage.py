import subprocess
import re
import os
import sys

def get_coverage():
    try:
        if not os.path.exists("build/cdd-tests"):
            subprocess.run(
                ["cmake", "-B", "build", "-S", ".", "-DCMAKE_BUILD_TYPE=Release"],
                check=True, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL
            )
            subprocess.run(
                ["cmake", "--build", "build", "-j4"],
                check=True, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL
            )
            
        result = subprocess.run(
            ["gcovr", "--print-summary", "-r", ".", "--filter", "src/", "--gcov-ignore-parse-errors=all"],
            capture_output=True, text=True, check=True
        )
        
        match_lines = re.search(r"lines:\s+([\d\.]+)%", result.stdout)
        match_funcs = re.search(r"functions:\s+([\d\.]+)%", result.stdout)
        match_branches = re.search(r"branches:\s+([\d\.]+)%", result.stdout)
        
        lines = float(match_lines.group(1)) if match_lines else 0.0
        funcs = float(match_funcs.group(1)) if match_funcs else 0.0
        # gcovr outputs 0.0% when there are 0 branches, which is considered 100% covered in this context since there are no branches to miss.
        # But wait, if gcovr outputs branches: 0.0% (0 out of 0), that should be treated as 100% because no branches are uncovered.
        # Let's extract the "taken" vs "total" branches if possible.
        branches = 0.0
        if match_branches:
            branch_match = re.search(r"branches:\s+[\d\.]+%\s+\((\d+)\s+out\s+of\s+(\d+)\)", result.stdout)
            if branch_match:
                taken = int(branch_match.group(1))
                total = int(branch_match.group(2))
                branches = 100.0 if total == 0 else (taken / total) * 100.0
            else:
                branches = float(match_branches.group(1))
        
        return lines, funcs, branches
            
    except Exception as e:
        print(f"Error calculating coverage: {e}")

    return 0.0, 0.0, 0.0

if __name__ == "__main__":
    lines, funcs, branches = get_coverage()
    print(f"Lines: {lines:.2f}%, Functions: {funcs:.2f}%, Branches: {branches:.2f}%")
    if lines < 100.0 or funcs < 100.0 or branches < 100.0:
        sys.exit(1)
