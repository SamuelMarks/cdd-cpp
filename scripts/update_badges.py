#!/usr/bin/env python3
"""Delegate to tools/update_badges.py for README badge updates."""

import os
import subprocess
import sys


def main():
    repo_root = os.path.abspath(os.path.join(os.path.dirname(__file__), ".."))
    tools_script = os.path.join(repo_root, "tools", "update_badges.py")
    if len(sys.argv) == 3:
        subprocess.run(
            [sys.executable, tools_script, sys.argv[1], sys.argv[2]],
            cwd=repo_root,
            check=True,
        )
    else:
        test_res = subprocess.run(
            [sys.executable, "tools/test_coverage.py"],
            cwd=repo_root,
            capture_output=True,
            text=True,
        )
        test_cov = test_res.stdout.strip() if test_res.returncode == 0 else "0"
        doc_res = subprocess.run(
            [sys.executable, "tools/doc_coverage.py"],
            cwd=repo_root,
            capture_output=True,
            text=True,
        )
        doc_cov = doc_res.stdout.strip() if doc_res.returncode == 0 else "0"
        subprocess.run(
            [sys.executable, tools_script, test_cov, doc_cov], cwd=repo_root, check=True
        )


if __name__ == "__main__":
    main()
