// Explicit standard includes also permit an x86 build on Mac.
#include <algorithm>
#include <cassert>
#include <climits>
#include <iostream>
#include <random>
#include <vector>
using namespace std;
using ll = long long;
#define rep(i,a,b) for (int i = a; i < (b); ++i)

#if defined(__x86_64__) || defined(__i386__)
#include "../../content/various/SIMD.h"

void check(vector<short> a, vector<short> b) {
	ll want = 0;
	rep(i,0,(int)a.size()) if (a[i] < b[i])
		want += ll(a[i])*b[i];
	assert(example_filteredDotProduct((int)a.size(),
		a.data(), b.data()) == want);
}

int main() {
#ifndef SIMD_FORCE_TEST // Rosetta can hide AVX2 in CPUID.
	if (!__builtin_cpu_supports("avx2")) {
		cout << "Skipped: AVX2 required" << endl;
		return 0;
	}
#endif
	rep(n,0,65) {
		check(vector<short>(n,-1), vector<short>(n,1));
		check(vector<short>(n,SHRT_MIN),
			vector<short>(n,SHRT_MAX));
		check(vector<short>(n,SHRT_MIN),
			vector<short>(n,SHRT_MIN+1));
		check(vector<short>(n,SHRT_MAX-1),
			vector<short>(n,SHRT_MAX));
	}
	mt19937 rng(20261008);
	uniform_int_distribution<int> value(SHRT_MIN,SHRT_MAX);
	rep(it,0,10000) {
		int n = (int)(rng()%513);
		vector<short> a(n), b(n);
		rep(i,0,n) a[i] = (short)value(rng),
			b[i] = (short)value(rng);
		check(a,b);
	}
	cout << "Tests passed!" << endl;
}
#else
int main() { cout << "Skipped: x86 AVX2 required" << endl; }
#endif
