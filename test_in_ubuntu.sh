#!/bin/bash
sudo apt-get update && sudo apt-get install -y cmake build-essential python3 curl git python3-pip gcovr
pip3 install --break-system-packages pre-commit gcovr || pip3 install pre-commit gcovr
python3 tools/test_coverage.py
