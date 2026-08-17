#!/usr/bin/env bash
SCRIPT_DIR=$(cd "$(dirname "$0")" && pwd)
# shellcheck source=cxx.sh
. "$SCRIPT_DIR/cxx.sh"

mkdir -p build
FILE="$1"
echo "
#include \"../content/contest/template.cpp\"
#include \"../$FILE\"
" >build/temp.cpp
$CXX -Wall -Wextra -Wfatal-errors -Wconversion -std=c++20 build/temp.cpp && rm -f a.out build/temp.cpp
