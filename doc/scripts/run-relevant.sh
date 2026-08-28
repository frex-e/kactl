#!/usr/bin/env bash
# Run stress tests that depend on files changed vs BASE (default: origin/main).
# Usage: run-relevant.sh [DIR]
set -euo pipefail
DIR=${1:-.}
SCRIPT_DIR=$(cd "$(dirname "$0")" && pwd)
REPO=$(cd "$DIR" && pwd)
cd "$REPO"

args=()
if [ -n "${BASE:-}" ]; then
	args+=(--base "$BASE")
fi
if [ -n "${STRESS_CHANGED+x}" ]; then
	args+=(--changed)
	if [ -n "${STRESS_CHANGED}" ]; then
		# shellcheck disable=SC2206
		changed_files=($STRESS_CHANGED)
		args+=("${changed_files[@]}")
	fi
fi

output=$(python3 -m tools.stress_select --repo "$REPO" --mode-line "${args[@]}")
mapfile -t lines <<< "$output"
mode=${lines[0]:-}
tests=("${lines[@]:1}")

case "$mode" in
	all)
		exec "$SCRIPT_DIR/run-all.sh" "$DIR"
		;;
	skip)
		echo "No relevant stress tests; skipping."
		exit 0
		;;
	subset)
		if [ ${#tests[@]} -eq 0 ]; then
			echo "No relevant stress tests; skipping."
			exit 0
		fi
		exec "$SCRIPT_DIR/run-all.sh" "$DIR" "${tests[@]}"
		;;
	*)
		echo "select-stress-tests: unexpected mode ${mode:-<empty>}" >&2
		exit 1
		;;
esac
