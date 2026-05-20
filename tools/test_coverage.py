import re

def get_coverage():
    try:
        with open('COVERAGE.md', 'r') as f:
            content = f.read()
    except FileNotFoundError:
        return "0.0"

    # Count how many checkboxes are checked vs total
    total = len(re.findall(r'\|\s*\[([ x])\]\s*\|', content))
    checked = len(re.findall(r'\|\s*\[x\]\s*\|', content))

    if total == 0:
        return "0.0"

    return f"{(checked / total) * 100:.2f}"

if __name__ == "__main__":
    print(get_coverage())
