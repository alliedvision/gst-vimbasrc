#!/usr/bin/env sh
set -o verbose
# ============================================================================
cmake -S . -B build -DVIMBA_HOME=$VIMBA_HOME
# ============================================================================
cmake --build build
# ============================================================================
