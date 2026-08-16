# Shared compiler pick for compile/test scripts. Source this file.
# Honor $CXX if set. Prefer Homebrew g++-N on macOS; use g++ on Linux.
# Skip a `g++` that is actually clang (Apple's /usr/bin/g++).
if [ -z "${CXX:-}" ]; then
	for cand in g++-15 g++-14 g++-13 g++; do
		if ! command -v "$cand" >/dev/null 2>&1; then
			continue
		fi
		if [ "$cand" = g++ ] && "$cand" --version 2>/dev/null | grep -qi clang; then
			continue
		fi
		CXX=$cand
		break
	done
fi
if [ -z "${CXX:-}" ]; then
	echo "No GCC found (tried g++-15, g++-14, g++-13, g++). Set CXX=..." >&2
	exit 1
fi
export CXX
