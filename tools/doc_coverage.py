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
    """Collect contiguous preceding /// or /** ... */ comment lines above idx, stepping over leading return-type continuation lines."""
    j = idx - 1
    while j >= 0:
        s = lines[j].strip()
        if s == "":
            break
        if (
            s.startswith("///")
            or s.startswith("/**")
            or s.startswith("*")
            or s.startswith("// NOLINT")
        ):
            break
        if (
            s.endswith(";")
            or s.endswith("{")
            or s.endswith("}")
            or s.endswith(":")
            or s.startswith("#")
        ):
            break
        # Return-type or template continuation line immediately preceding function name
        j -= 1

    doc_lines = []
    while j >= 0:
        s = lines[j].strip()
        if s.startswith("///") or s.startswith("/**") or s.startswith("*"):
            doc_lines.append(s)
            j -= 1
        elif s.startswith("// NOLINT") or s.startswith("template"):
            j -= 1
        else:
            break
    doc_lines.reverse()
    return NL.join(doc_lines)


def _extract_param_names(sig):
    """Extract parameter identifiers from a C++ function signature."""
    lparen = sig.find("(")
    if lparen == -1:
        return []
    depth = 0
    rparen = -1
    for idx in range(lparen, len(sig)):
        if sig[idx] == "(":
            depth += 1
        elif sig[idx] == ")":
            depth -= 1
            if depth == 0:
                rparen = idx
                break
    if rparen == -1:
        return []
    inner = sig[lparen + 1 : rparen].strip()
    if not inner or inner == "void":
        return []
    # Split on top-level commas (outside <...>)
    parts = []
    depth = 0
    start = 0
    for idx, ch in enumerate(inner):
        if ch in "<(":
            depth += 1
        elif ch in ">)":
            depth = max(0, depth - 1)
        elif ch == "," and depth == 0:
            parts.append(inner[start:idx])
            start = idx + 1
    parts.append(inner[start:])

    names = []
    for p in parts:
        p = p.split("=")[0].strip()
        if not p:
            continue
        # Remove trailing array brackets if any
        p = re.sub(r"\[.*\]$", "", p).strip()
        tok_m = re.search(r"([A-Za-z_]\w*)$", p)
        if not tok_m:
            continue
        candidate = tok_m.group(1)
        if candidate in ("nullptr_t", "size_t", "int64_t", "uint64_t", "string", "string_view"):
            continue
        # Check if candidate is actually a type without parameter name (e.g. `const Foo &` or `int`)
        before = p[: tok_m.start()].strip()
        if (
            not before
            or before.endswith("::")
            or before in ("const", "struct", "class", "typename", "enum")
        ):
            continue
        names.append(candidate)
    return names


def audit_source_tree(src_dir="src", verbose=False):
    """Audit C++ source tree for 100% file, module (namespace), class/struct/enum, field, function, and arg (@param) Doxygen coverage."""
    total_items = 0
    documented_items = 0
    undocumented_details = []

    documented_namespaces = set()

    for root, _, files in os.walk(src_dir):
        if "tests" in root.split(os.sep):
            continue
        for file in sorted(files):
            if file in ("httplib.h", "exceptions_stub.cpp"):
                continue
            if not (file.endswith(".hpp") or file.endswith(".cpp")):
                continue

            filepath = os.path.join(root, file)
            with open(filepath, "r", encoding="utf-8", errors="ignore") as f:
                content = f.read()

            lines = content.splitlines()

            # 1. File-level @file & @brief check
            total_items += 1
            header_chunk = NL.join(lines[:15])
            if "@file" in header_chunk and "@brief" in header_chunk:
                documented_items += 1
            else:
                undocumented_details.append(f"{filepath}:1 (missing @file / @brief)")

            # Track namespace docstrings across headers
            for m in re.finditer(
                r"///\s*@namespace\s+([a-zA-Z0-9_:]+)\s*" + NL + r"///\s*@brief\s+\S+",
                content,
            ):
                documented_namespaces.add(m.group(1))

            # 2. Scan declarations in both .hpp and .cpp files
            in_raw_string = False
            brace_depth = 0
            class_brace_depths = []

            i = 0
            while i < len(lines):
                line = lines[i]
                stripped = line.strip()

                # Track raw string literals R"(...)"
                if not in_raw_string and 'R"(' in line:
                    if ')"' not in line.split('R"(', 1)[1]:
                        in_raw_string = True
                        i += 1
                        continue
                elif in_raw_string:
                    if ')"' in line:
                        in_raw_string = False
                    i += 1
                    continue

                if (
                    not stripped
                    or stripped.startswith("//")
                    or stripped.startswith("/*")
                    or stripped.startswith("*")
                    or stripped.startswith("#")
                ):
                    i += 1
                    continue

                # Check if we are entering a struct / class / enum
                type_match = re.match(
                    r"^(?:template\s*<[^>]*>\s*)?(struct|class|enum\s+class|enum)\s+([A-Za-z_]\w*)\b([^;]*)$",
                    stripped,
                )
                if type_match and "{" in stripped:
                    kind = type_match.group(1)
                    name = type_match.group(2)
                    total_items += 1
                    doc = _extract_preceding_docblock(lines, i)
                    if "@brief" in doc or "///<" in stripped:
                        documented_items += 1
                    else:
                        undocumented_details.append(
                            f"{filepath}:{i+1} ({kind} {name} missing @brief)"
                        )

                # Track brace depth for top-level / class-level vs function-body-level
                open_b = stripped.count("{")
                close_b = stripped.count("}")

                # Check for function / method declarations at namespace or class scope
                is_at_decl_scope = brace_depth <= len(class_brace_depths) + 1
                if file.endswith(".hpp") and is_at_decl_scope:
                    if (
                        "(" in stripped
                        and not stripped.startswith("if ")
                        and not stripped.startswith("if(")
                        and not stripped.startswith("for ")
                        and not stripped.startswith("for(")
                        and not stripped.startswith("while ")
                        and not stripped.startswith("while(")
                        and not stripped.startswith("switch ")
                        and not stripped.startswith("switch(")
                        and not stripped.startswith("return ")
                        and not stripped.startswith("using ")
                        and not stripped.startswith("typedef ")
                        and "MOCK_METHOD" not in stripped
                    ):
                        sig_lines = [stripped]
                        k = i
                        while (
                            not (sig_lines[-1].endswith(";") or "{" in sig_lines[-1])
                            and k + 1 < len(lines)
                            and k - i < 8
                        ):
                            k += 1
                            sig_lines.append(lines[k].strip())
                        full_sig = " ".join(sig_lines)
                        fn_match = re.match(
                            r"^(?:template\s*<[^>]*>\s*)?(?:inline\s+|static\s+|virtual\s+|explicit\s+|constexpr\s+|friend\s+)*(?:[A-Za-z_~][\w:<>,\s\*&]*?\s+)?([A-Za-z_~]\w*|operator\S+)\s*\([^;{}]*\)\s*(?:const\s*)?(?:noexcept\s*)?(?:override\s*)?(?:=\s*(?:default|delete|0)\s*)?[;{]",
                            full_sig,
                        )
                        if fn_match:
                            fn_name = fn_match.group(1)
                            if fn_name not in ("if", "for", "while", "switch", "catch"):
                                total_items += 1
                                doc = _extract_preceding_docblock(lines, i)
                                has_brief = (
                                    "@brief" in doc
                                    or "///<" in full_sig
                                    or "/**" in doc
                                )
                                param_names = (
                                    []
                                    if fn_name.startswith("operator")
                                    else _extract_param_names(full_sig)
                                )
                                missing_params = [
                                    p
                                    for p in param_names
                                    if not re.search(
                                        rf"@param(?:\[\w+\])?\s+{re.escape(p)}\b", doc
                                    )
                                ]
                                if has_brief and not missing_params:
                                    documented_items += 1
                                else:
                                    if not has_brief:
                                        undocumented_details.append(
                                            f"{filepath}:{i+1} (function {fn_name} missing @brief)"
                                        )
                                    if missing_params:
                                        undocumented_details.append(
                                            f"{filepath}:{i+1} (function {fn_name} missing @param for {', '.join(missing_params)})"
                                        )
                                open_b = full_sig.count("{")
                                close_b = full_sig.count("}")
                                i = k

                    # Check struct/class member fields in headers
                    elif (
                        class_brace_depths
                        and brace_depth == class_brace_depths[-1]
                        and stripped.endswith(";")
                        and "(" not in stripped
                        and not stripped.startswith("using ")
                        and not stripped.startswith("typedef ")
                        and not stripped.startswith("friend ")
                        and not stripped.startswith("public:")
                        and not stripped.startswith("private:")
                        and not stripped.startswith("protected:")
                        and not stripped.startswith("return ")
                        and not stripped.startswith("break;")
                        and not stripped.startswith("continue;")
                    ):
                        fld_match = re.match(
                            r"^(?:static\s+|constexpr\s+|inline\s+|mutable\s+)*[A-Za-z_][\w:<>,\s\*&]+\s+([A-Za-z_]\w*)(?:\s*=\s*[^;]+|\s*\{[^;]*\})?\s*;(?:\s*///<.*)?$",
                            stripped,
                        )
                        if fld_match:
                            fld_name = fld_match.group(1)
                            total_items += 1
                            doc = _extract_preceding_docblock(lines, i)
                            if "///<" in line or "@brief" in doc or "///" in doc:
                                documented_items += 1
                            else:
                                undocumented_details.append(
                                    f"{filepath}:{i+1} (field {fld_name} missing docstring)"
                                )

                elif file.endswith(".cpp") and brace_depth <= 1:
                    # Top-level function definitions in .cpp files
                    if (
                        "(" in stripped
                        and not stripped.startswith("if ")
                        and not stripped.startswith("if(")
                        and not stripped.startswith("for ")
                        and not stripped.startswith("for(")
                        and not stripped.startswith("while ")
                        and not stripped.startswith("while(")
                        and not stripped.startswith("switch ")
                        and not stripped.startswith("switch(")
                        and not stripped.startswith("return ")
                        and not stripped.startswith("using ")
                        and not stripped.startswith("typedef ")
                    ):
                        sig_lines = [stripped]
                        k = i
                        while (
                            ")" not in sig_lines[-1]
                            and k + 1 < len(lines)
                            and k - i < 8
                        ):
                            k += 1
                            sig_lines.append(lines[k].strip())
                        full_sig = " ".join(sig_lines)
                        fn_match = re.match(
                            r"^(?:template\s*<[^>]*>\s*)?(?:inline\s+|static\s+|constexpr\s+)*(?:[A-Za-z_][\w:<>,\s\*&]*?\s+)?([A-Za-z_][\w:]*|operator\S+)\s*\([^;{}]*\)\s*(?:const\s*)?(?:noexcept\s*)?\{",
                            full_sig,
                        )
                        if fn_match:
                            fn_name = fn_match.group(1)
                            if fn_name not in ("if", "for", "while", "switch", "catch"):
                                total_items += 1
                                doc = _extract_preceding_docblock(lines, i)
                                if "@brief" in doc or "///" in doc:
                                    documented_items += 1
                                else:
                                    undocumented_details.append(
                                        f"{filepath}:{i+1} (.cpp function {fn_name} missing @brief)"
                                    )
                                open_b = full_sig.count("{")
                                close_b = full_sig.count("}")
                                i = k

                if type_match and "{" in stripped:
                    class_brace_depths.append(brace_depth + open_b)

                brace_depth += open_b - close_b
                while class_brace_depths and brace_depth < class_brace_depths[-1]:
                    class_brace_depths.pop()

                i += 1

    # 3. Module (namespace) coverage check
    for ns in REQUIRED_NAMESPACES:
        total_items += 1
        if ns in documented_namespaces:
            documented_items += 1
        else:
            undocumented_details.append(f"namespace {ns} (missing @namespace / @brief)")

    if verbose and undocumented_details:
        for item in undocumented_details:
            print(f"UNDOCUMENTED: {item}", file=sys.stderr)

    return total_items, documented_items, undocumented_details


def check_doc_coverage(verbose=False):
    total_items, documented_items, _ = audit_source_tree("src", verbose=verbose)
    if total_items == 0:
        return 100.0
    coverage = (documented_items / total_items) * 100.0
    if verbose:
        print(f"Total: {total_items}, Documented: {documented_items}")
    print(f"{coverage:.2f}")
    return coverage


def _run_self_tests():
    import tempfile
    import shutil

    tmp = tempfile.mkdtemp()
    try:
        os.makedirs(os.path.join(tmp, "sub"))
        with open(os.path.join(tmp, "sub", "good.hpp"), "w") as f:
            f.write(
                NL.join(
                    [
                        "/// @file good.hpp",
                        "/// @brief Brief.",
                    ]
                    + [
                        f"/// @namespace {ns}" + NL + "/// @brief NS."
                        for ns in REQUIRED_NAMESPACES
                    ]
                    + [
                        "/// @brief S.",
                        "struct S {",
                        "  int x = 0; ///< Field x.",
                        "  /// @brief Fn.",
                        "  /// @param a Arg a.",
                        "  void foo(int a) noexcept;",
                        "};",
                        "",
                    ]
                )
            )
        with open(os.path.join(tmp, "sub", "good.cpp"), "w") as f:
            f.write(
                NL.join(
                    [
                        "/// @file good.cpp",
                        "/// @brief Brief.",
                        "/// @brief Impl.",
                        "/// @param a Arg a.",
                        "void foo(int a) noexcept {",
                        "}",
                        "",
                    ]
                )
            )
        tot, doc, details = audit_source_tree(tmp, verbose=False)
        assert tot == doc, f"Expected 100% on good tree, got {details}"

        with open(os.path.join(tmp, "sub", "bad.hpp"), "w") as f:
            f.write(
                NL.join(
                    [
                        "struct Undoc {",
                        "  int y;",
                        "  void bar(int z);",
                        "};",
                        "",
                    ]
                )
            )
        tot2, doc2, details2 = audit_source_tree(tmp, verbose=False)
        assert doc2 < tot2 and len(details2) >= 4
    finally:
        shutil.rmtree(tmp)


if __name__ == "__main__":
    if "--self-test" in sys.argv:
        _run_self_tests()
        print("doc_coverage.py self-tests passed.")
        sys.exit(0)
    verbose_flag = "--verbose" in sys.argv or "-v" in sys.argv
    cov = check_doc_coverage(verbose_flag)
    if cov < 100.0:
        sys.exit(1)
