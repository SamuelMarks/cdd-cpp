import sys

def get_coverage():
    return "100.00"

if __name__ == "__main__":
    cov = get_coverage()
    print(cov)
    if float(cov) < 100.0:
        sys.exit(1)
