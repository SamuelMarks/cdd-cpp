import os
import re
import sys

NL = chr(10)

REQUIRED_NAMESPACES = [
    "cdd_cpp::arazzo",
    "cdd_cpp::cli",
    "cdd_cpp::client_sdk",
    "cdd_cpp::client_sdk_cli",
    "cdd_cpp::docstrings",
    "cdd_cpp::functions",
    "cdd_cpp::google_discovery",
    "cdd_cpp::mcp",
    "cdd_cpp::mocks",
    "cdd_cpp::models",
    "cdd_cpp::openapi",
    "cdd_cpp::openapi::upgraders",
    "cdd_cpp::orm",
    "cdd_cpp::server",
    "cdd_cpp::utils",
]


def _extract_preceding_docblock(lines, idx):
    """Collect contiguous preceding /// or /** ... */ comment lines above idx."""
    j = idx - 1
    while j >= 0 and (
        lines[j].strip().startswith("template")
        or lines[j].strip().startswith("// NOLINT")
    ):
        j -= 1
    doc_lines = []
    while j >= 0:
        s = lines[j].strip()
        if s.startswith("///") or s.startswith("/**") or s.startswith("*"):
            doc_lines.append(s)
            j -= 1
        elif s.startswith("// NOLINT"):
            j -= 1
        else:
            break
    doc_lines.reverse()
    return NL.join(doc_lines)


def _extract_param_names(param_str):
    """Extract parameter identifier names from a C++ parameter list string."""
    param_str = param_str.strip()
    if not param_str or param_str == "void":
        return []
    parts = []
    depth = 0
    cur = []
    for ch in param_str:
        if ch in "<(":
            depth += 1
            cur.append(ch)
        elif ch in ">)":
            depth -= 1
            cur.append(ch)
        elif ch == "," and depth == 0:
            parts.append("".join(cur).strip())
            cur = []
        else:
            cur.append(ch)
    if cur:
        parts.append("".join(cur).strip())

    names = []
    for p in parts:
        if "=" in p:
            p = p.split("=", 1)[0].strip()
        if p.endswith("&") or p.endswith("*") or p == "std::nullptr_t":
            continue
        m = re.search(r"([a-zA-Z_][a-zA-Z0-9_]*)\s*$", p)
        if m:
            candidate = m.group(1)
            if candidate not in (
                "int",
                "double",
                "bool",
                "void",
                "char",
                "string",
                "string_view",
                "nullptr_t",
            ):
                names.append(candidate)
    return names


def audit_file_content(filepath, content, is_header, verbose=False):
    """Audit a single file's content and return (total_items, documented_items)."""
    total = 0
    documented = 0

    # 1. File-level docstring check (@file and @brief)
    total += 1
    first_lines = NL.join(content.splitlines()[:10])
    if (
        "@file" in first_lines
        and "@brief" in first_lines
        and "Auto-generated doc." not in content
        and "Internal helper or implementation for" not in content
    ):
        documented += 1
    elif verbose:
        print(f"UNDOC FILE: {filepath}")

    if not is_header:
        return total, documented

    lines = content.splitlines()
    i = 0
    while i < len(lines):
        stripped = lines[i].strip()
        if (
            not stripped
            or stripped.startswith("//")
            or stripped.startswith("/*")
            or stripped.startswith("*")
            or stripped.startswith("#")
            or stripped.startswith("}")
            or stripped.startswith(":")
        ):
            i += 1
            continue

        # 2. Check struct / class / enum class declarations
        if re.match(
            r"^(?:struct|class|enum\s+class)\s+[a-zA-Z_][a-zA-Z0-9_]*\s*(?:final\s*)?(?:[:,{]|$)",
            stripped,
        ):
            if not stripped.endswith(";"):
                total += 1
                docblock = _extract_preceding_docblock(lines, i)
                if "@brief" in docblock and "Auto-generated doc." not in docblock:
                    documented += 1
                elif verbose:
                    print(f"UNDOC TYPE: {filepath}:{i + 1}: {stripped}")
            i += 1
            continue

        # 3. Check function / method declarations (single-line or multi-line)
        if (
            "(" in stripped
            and not any(
                stripped.startswith(kw)
                for kw in (
                    "if ",
                    "if(",
                    "else ",
                    "for ",
                    "for(",
                    "while ",
                    "while(",
                    "switch ",
                    "switch(",
                    "return ",
                    "case ",
                    "using ",
                    "typedef ",
                )
            )
            and "=" not in stripped.split("(", 1)[0]
        ):
            sig_start = i
            sig_lines = [stripped]
            while (
                "(" in " ".join(sig_lines)
                and ")" not in " ".join(sig_lines)
                and i + 1 < len(lines)
            ):
                i += 1
                sig_lines.append(lines[i].strip())
            full_sig = " ".join(sig_lines)

            m_fn = re.match(
                r"^(?:template\s*<([^>]+)>\s*)?(?:virtual\s+)?(?:(?:inline|static|explicit|constexpr)\s+)*([a-zA-Z0-9_:\-<>,\s\*&]+?)\s+([a-zA-Z_~][a-zA-Z0-9_:]*|operator[^\s(]+)\s*\(([^)]*)\)\s*(?:const)?\s*(?:noexcept)?\s*(?:=\s*0|=\s*default|=\s*delete)?\s*(?:;|\{|$)",
                full_sig,
            )
            if m_fn:
                tparams_raw, ret_type, fn_name, params_raw = m_fn.groups()
                ret_type = ret_type.strip()
                fn_name = fn_name.strip()
                if (
                    fn_name not in ("if", "for", "while", "switch", "return", "sizeof")
                    and "<<" not in ret_type
                ):
                    docblock = _extract_preceding_docblock(lines, sig_start)
                    total += 1
                    if "@brief" in docblock and "Auto-generated doc." not in docblock:
                        documented += 1
                    elif verbose:
                        print(f"UNDOC FUNC: {filepath}:{sig_start + 1}: {full_sig}")

                    if tparams_raw:
                        for tp in re.findall(
                            r"(?:typename|class)\s+([a-zA-Z_][a-zA-Z0-9_]*)",
                            tparams_raw,
                        ):
                            total += 1
                            if f"@tparam {tp}" in docblock:
                                documented += 1
                            elif verbose:
                                print(
                                    f"UNDOC TPARAM '{tp}': {filepath}:{sig_start + 1}: {fn_name}"
                                )

                    if not fn_name.startswith("operator"):
                        for pname in _extract_param_names(params_raw):
                            total += 1
                            if f"@param {pname}" in docblock:
                                documented += 1
                            elif verbose:
                                print(
                                    f"UNDOC PARAM '{pname}': {filepath}:{sig_start + 1}: {fn_name}"
                                )

                        if ret_type != "void" and not fn_name.startswith("~"):
                            total += 1
                            if "@return" in docblock:
                                documented += 1
                            elif verbose:
                                print(
                                    f"UNDOC RETURN: {filepath}:{sig_start + 1}: {fn_name} -> {ret_type}"
                                )

        i += 1

    return total, documented


def check_doc_coverage(verbose=False):
    total = 0
    documented = 0
    all_headers_text = []

    for root, _, files in os.walk("src"):
        if "tests" in root:
            continue
        for file in sorted(files):
            if file in ("httplib.h", "exceptions_stub.cpp"):
                continue
            if not (file.endswith(".hpp") or file.endswith(".cpp")):
                continue

            filepath = os.path.join(root, file)
            with open(filepath, "r") as f:
                content = f.read()

            if file.endswith(".hpp"):
                all_headers_text.append(content)

            f_tot, f_doc = audit_file_content(
                filepath, content, file.endswith(".hpp"), verbose
            )
            total += f_tot
            documented += f_doc

    combined_headers = NL.join(all_headers_text)
    for ns in REQUIRED_NAMESPACES:
        total += 1
        if f"@namespace {ns}" in combined_headers:
            documented += 1
        elif verbose:
            print(f"UNDOC NAMESPACE: {ns}")

    if total == 0:
        total = 1
    if verbose:
        print(f"Total: {total}, Documented: {documented}")
    cov = documented / total * 100
    print(f"{cov:.2f}")
    return cov


def run_self_tests():
    """Verify that audit_file_content detects missing file, class, func, param, and return docs."""
    tot, doc = audit_file_content("bad.hpp", "struct Undoc {};" + NL, True, False)
    assert doc < tot, "Expected undocumented file and struct to fail"

    good_hdr = NL.join(
        [
            "/// @file good.hpp",
            "/// @brief Brief file.",
            "/// @brief Good struct.",
            "struct Good {",
            "  /// @brief Add two ints.",
            "  /// @param a First int.",
            "  /// @return Sum.",
            "  int add(int a, int b) noexcept;",
            "};",
        ]
    )
    tot2, doc2 = audit_file_content("good.hpp", good_hdr, True, False)
    assert doc2 == tot2 - 1, "Expected missing @param b to be detected"


if __name__ == "__main__":
    if "--self-test" in sys.argv:
        run_self_tests()
        print("doc_coverage.py self-tests passed.")
        sys.exit(0)
    verbose_flag = "--verbose" in sys.argv or "-v" in sys.argv
    cov = check_doc_coverage(verbose_flag)
    if cov < 100.0:
        sys.exit(1)
