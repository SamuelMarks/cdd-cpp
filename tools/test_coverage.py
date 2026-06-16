import subprocess
import os
import sys
import json

def get_coverage():
    try:
        if not os.path.exists("build/cdd-tests"):
            subprocess.run(
                ["cmake", "-B", "build", "-S", ".", "-DCMAKE_BUILD_TYPE=Debug", "-DCOVERAGE=ON"],
                check=True, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL
            )
            subprocess.run(
                ["cmake", "--build", "build", "-j4"],
                check=True, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL
            )
            
        result = subprocess.run(
            ["gcovr", "-r", ".", "--filter", "src/", "--json", "--gcov-ignore-parse-errors=all"],
            capture_output=True, text=True, check=True
        )
        
        data = json.loads(result.stdout)
        total_lines = 0
        covered_lines = 0
        total_funcs = 0
        covered_funcs = 0
        total_branches = 0
        covered_branches = 0
        
        for file in data['files']:
            # Ignore absolute paths that are artifacts of test harnesses
            if file['file'].startswith('/'):
                continue
                
            for line in file['lines']:
                if line.get('gcovr/excluded', False):
                    continue
                total_lines += 1
                if line['count'] > 0:
                    covered_lines += 1
                    
                branches = line.get('branches', [])
                for b in branches:
                    if b.get('gcovr/excluded', False):
                        continue
                    total_branches += 1
                    if b['count'] > 0:
                        covered_branches += 1
            
            for func in file['functions']:
                if func.get('gcovr/excluded', False):
                    continue
                total_funcs += 1
                if func['execution_count'] > 0:
                    covered_funcs += 1
                
            # If no branches, handle as 100%
            
        lines = (covered_lines / total_lines * 100.0) if total_lines > 0 else 100.0
        funcs = (covered_funcs / total_funcs * 100.0) if total_funcs > 0 else 100.0
        branches = (covered_branches / total_branches * 100.0) if total_branches > 0 else 100.0
        
        return lines, funcs, branches
            
    except Exception as e:
        print(f"Error calculating coverage: {e}")

    return 0.0, 0.0, 0.0

if __name__ == "__main__":
    lines, funcs, branches = get_coverage()
    print(f"Lines: {lines:.2f}%, Functions: {funcs:.2f}%, Branches: {branches:.2f}%")
    if lines < 100.0 or funcs < 100.0 or branches < 100.0:
        sys.exit(1)
