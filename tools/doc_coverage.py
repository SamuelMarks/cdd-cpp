import os, sys, re


def check_doc_coverage(verbose=False):
    total = 0
    documented = 0
    for root, _, files in os.walk("src"):
        if "tests" in root:
            continue
        for file in files:
            if file == "httplib.h" or file == "exceptions_stub.cpp":
                continue
            if not (file.endswith(".hpp") or file.endswith(".cpp")):
                continue

            filepath = os.path.join(root, file)
            with open(filepath, "r") as f:
                content = f.read()

            # 1. File-level docstring check (@file or leading ///)
            total += 1
            first_lines = "\n".join(content.splitlines()[:10])
            if ("@file" in first_lines or "///" in first_lines) and "Auto-generated doc." not in first_lines:
                documented += 1
            elif verbose:
                print(f"UNDOC FILE: {filepath}")

            if not file.endswith(".hpp"):
                continue

            lines = content.split("\n")
            for i, line in enumerate(lines):
                stripped = line.strip()
                if not stripped:
                    continue
                if (
                    stripped.startswith("//")
                    or stripped.startswith("/*")
                    or stripped.startswith("*")
                    or stripped.startswith("#")
                    or stripped.startswith("}")
                    or stripped.startswith(":")
                ):
                    continue

                is_decl = False
                if re.match(r"^(?:struct|class|enum\s+class)\s+[a-zA-Z_][a-zA-Z0-9_]*\s*[{:]", stripped):
                    is_decl = True
                elif re.match(
                    r"^(?:template\s*<[^>]+>\s*)?(?:virtual\s+)?(?:(?:inline|static|explicit|constexpr)\s+)?(?:[a-zA-Z_:\-<>]+\s+)+[a-zA-Z_~:]+\s*\([^;{]*\)\s*(?:const)?\s*(?:noexcept)?\s*(?:=\s*0|=\s*default|=\s*delete)?\s*(?:;|{)",
                    stripped,
                ):
                    if (
                        " main(" in stripped
                        or "return " in stripped
                        or "if (" in stripped
                        or "else " in stripped
                        or "for (" in stripped
                        or "while (" in stripped
                        or "switch (" in stripped
                    ):
                        continue
                    is_decl = True

                if is_decl:
                    total += 1
                    prev_idx = i - 1
                    if prev_idx >= 0 and lines[prev_idx].strip().startswith("template"):
                        prev_idx -= 1
                    prev_line = lines[prev_idx] if prev_idx >= 0 else ""
                    if (
                        ("///" in prev_line or "*/" in prev_line or "//" in prev_line)
                        and "Auto-generated doc." not in prev_line
                    ):
                        documented += 1
                    else:
                        if verbose:
                            print(f"UNDOC: {filepath}:{i + 1}: {stripped}")

    if total == 0:
        total = 1
    if verbose:
        print(f"Total: {total}, Documented: {documented}")
    cov = documented / total * 100
    print(f"{cov:.2f}")
    return cov


if __name__ == "__main__":
    verbose_flag = "--verbose" in sys.argv or "-v" in sys.argv
    cov = check_doc_coverage(verbose_flag)
    if cov < 100.0:
        sys.exit(1)
