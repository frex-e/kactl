#!/usr/bin/env bash
DIR=${1:-.}
SCRIPT_DIR=$(cd "$(dirname "$0")" && pwd)
# shellcheck source=cxx.sh
. "$SCRIPT_DIR/cxx.sh"
echo "Using CXX=$CXX"

# use a precompiled header for the template to improve perf
$CXX -Wall -Wextra -Wfatal-errors -Wconversion -std=c++17 -x c++-header $DIR/content/contest/template.cpp

SKIP_FILE=$(mktemp)
cleanup() {
	rm -f "$SKIP_FILE" \
		"$DIR/content/contest/template.cpp.gch" \
		"$DIR/content/contest/template.cpp.pch"
}
trap cleanup EXIT
cat "$SCRIPT_DIR/skip_headers" > "$SKIP_FILE"
# avx2 target pragmas only compile on x86
if ! echo | $CXX -dM -E -x c++ - 2>/dev/null | grep -Eq '__x86_64__|__i386__'; then
	printf 'Pragmas.h\nSIMD.h\n' >> "$SKIP_FILE"
fi

tests="$(find $DIR/content -name '*.h' | grep -vFf $SKIP_FILE)"
echo "skipped: "
find $DIR/content -name '*.h' | grep -Ff $SKIP_FILE
declare -i pass=0
declare -i fail=0
failHeaders=""
for test in $tests; do
    echo "$(basename $test): "
    $SCRIPT_DIR/test-compiles.sh $test
    retCode=$?
    if (($retCode != 0)); then
        echo $retCode
        fail+=1
        failHeaders="$failHeaders$test\n"
    else
        pass+=1
    fi
    echo
done
echo "$pass/$(($pass+$fail)) tests passed"
if (($pass == 0)); then
    echo "No tests found (make sure skip_headers doesn't have whitespace lines)"
    exit 1
elif (($fail == 0)); then
    echo "No tests failed"
    exit 0
else
    echo -e "These tests failed: \n $failHeaders"
    exit 1
fi
