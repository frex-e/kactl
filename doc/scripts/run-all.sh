#!/usr/bin/env bash
DIR=${1:-.}
SCRIPT_DIR=$(cd "$(dirname "$0")" && pwd)
# shellcheck source=cxx.sh
. "$SCRIPT_DIR/cxx.sh"
echo "Using CXX=$CXX"

# use a precompiled header for the template to improve perf
$CXX -Wall -Wfatal-errors -Wconversion -std=c++20 -O2 $DIR/stress-tests/utilities/template.h
trap "rm -f $DIR/stress-tests/utilities/template.h.gch $DIR/stress-tests/utilities/template.h.pch" EXIT

now() {
	# GNU date has %N; BSD date prints a literal N
	local t
	t=$(date +%s.%N 2>/dev/null) || true
	if [[ -z $t || $t == *N* ]]; then
		date +%s
	else
		echo "$t"
	fi
}

tests="$(find $DIR/stress-tests -name '*.cpp')"
declare -i pass=0
declare -i fail=0
failTests=""
ulimit -s 524288 # For 2-sat test
for test in $tests; do
    echo "$(basename $test): "
    start=$(now)
    $CXX -Wall -Wfatal-errors -Wconversion -std=c++20 -O2 $test && ./a.out
    retCode=$?
    if (($retCode != 0)); then
        echo "Failed with $retCode"
        fail+=1
        failTests="$failTests$test\n"
    else
        pass+=1
    fi
    end=$(now)
    runtime=$( echo "$end - $start" | bc -l )
    echo "Took $runtime seconds"
    rm -f a.out
    echo
done
echo "$pass/$(($pass+$fail)) tests passed"
if (($fail == 0)); then
    echo "No tests failed"
    exit 0
else
    echo -e "These tests failed: \n $failTests"
    exit 1
fi
