import os, sys, re

def check_doc_coverage(verbose=False):
    total = 0
    documented = 0
    pattern_struct = re.compile(r'^\s*(?:struct|class)\s+([a-zA-Z_][a-zA-Z0-9_]*)\s*[{:]', re.MULTILINE)
    for root, _, files in os.walk('src'):
        for file in files:
            if not file.endswith('.hpp') or 'tests' in root: continue
            
            with open(os.path.join(root, file), 'r') as f:
                content = f.read()
                
                lines = content.split('\n')
                for i, line in enumerate(lines):
                    stripped = line.strip()
                    if not stripped: continue
                    if stripped.startswith('//') or stripped.startswith('/*') or stripped.startswith('*') or stripped.startswith('#') or stripped.startswith('}'): continue
                    
                    is_decl = False
                    if ('struct ' in stripped or 'class ' in stripped) and '{' in stripped:
                        is_decl = True
                    elif re.match(r'^(?:template\s*<[^>]+>\s*)?(?:virtual\s+)?(?:(?:inline|static|explicit)\s+)?(?:[a-zA-Z_:-<>]+\s+)+[a-zA-Z_~:]+\s*\([^;{]*\)\s*(?:const)?\s*(?:noexcept)?\s*(?:=\s*0|=\s*default|=\s*delete)?\s*(?:;|{)', stripped):
                        if ' main(' in stripped or 'return ' in stripped or 'if (' in stripped or 'else ' in stripped or 'for (' in stripped or 'while (' in stripped or 'switch (' in stripped:
                            continue
                        is_decl = True
                        
                    if is_decl:
                        total += 1
                        if i > 0 and ('///' in lines[i-1] or '*/' in lines[i-1] or '//' in lines[i-1]):
                            documented += 1
                        else:
                            if verbose: print(f"UNDOC: {root}/{file}:{i+1}: {stripped}")
    
    if total == 0: total = 1
    if verbose:
        print(f"Total: {total}, Documented: {documented}")
    print(f"{documented/total*100:.2f}")

if __name__ == '__main__':
    check_doc_coverage(False)
