#!/usr/bin/env python3
"""Run host regression checks without npm dependencies or build artifacts in source."""
import argparse
import os
from pathlib import Path
import shutil
import subprocess
import tempfile

root = Path(__file__).resolve().parents[1]
parser = argparse.ArgumentParser()
parser.add_argument('--node', default=os.environ.get('NODE', 'node'))
parser.add_argument('--cxx', default=os.environ.get('CXX', shutil.which('clang++') or 'g++'))
parser.add_argument('--no-sanitizers', action='store_true')
args = parser.parse_args()

def run(command):
    subprocess.run([str(x) for x in command], check=True, cwd=root)

with tempfile.TemporaryDirectory(prefix='listlib-check-') as temp:
    temp = Path(temp)
    flags = ['-std=c++11', '-fno-exceptions', '-Wall', '-Wextra', '-Werror', '-g',
             '-I', root/'tests', '-I', root/'arduinoC/libraries/listlib']
    if not args.no_sanitizers:
        flags += ['-fsanitize=address,undefined', '-fno-omit-frame-pointer']
    run([args.cxx, *flags, root/'tests/test_listlib.cpp', '-o', temp/'test_listlib'])
    run([temp/'test_listlib'])
    run([shutil.which('python3') or 'python3', root/'tests/test_generator.py', temp/'generator.mjs'])
    run([args.node, temp/'generator.mjs'])
    run([args.cxx, *flags, '-fsyntax-only', temp/'generator.cpp'])
print('PASS: all host checks (C++11, exceptions disabled)')
