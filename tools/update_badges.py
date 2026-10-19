import re
import sys
import urllib.parse


def format_badge_value(cov):
    raw = urllib.parse.unquote(str(cov).strip())
    pct_matches = re.findall(r"(\d+(?:\.\d+)?)%", raw)
    if pct_matches:
        min_pct = min(float(p) for p in pct_matches)
        raw = f"{min_pct:.2f}%"
    else:
        raw = re.sub(r"%+$", "%", raw)
        if "%" not in raw:
            raw = f"{raw}%"
    escaped = raw.replace("-", "--").replace("_", "__")
    return urllib.parse.quote(escaped, safe="")


def update_readme(test_cov, doc_cov):
    with open("README.md", "r") as f:
        content = f.read()

    encoded_test_cov = format_badge_value(test_cov)
    encoded_doc_cov = format_badge_value(doc_cov)

    test_badge = f"![Test Coverage](https://img.shields.io/badge/test_coverage-{encoded_test_cov}-brightgreen.svg)"
    doc_badge = f"![Doc Coverage](https://img.shields.io/badge/doc_coverage-{encoded_doc_cov}-brightgreen.svg)"

    content = re.sub(r"!\[Test Coverage\]\([^)]+\)", test_badge, content)
    content = re.sub(r"!\[Doc Coverage\]\([^)]+\)", doc_badge, content)

    if test_badge not in content:
        content = re.sub(
            r"(cdd-cpp(?:: C\+\+ ⇄ OpenAPI)?\n=+\n)",
            r"\1\n" + f"[{test_badge}](#)\n[{doc_badge}](#)\n",
            content,
        )

    with open("README.md", "w") as f:
        f.write(content)
    print("README.md updated with badges.")


if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Usage: python3 update_badges.py <test_cov> <doc_cov>")
        sys.exit(1)
    update_readme(sys.argv[1], sys.argv[2])
