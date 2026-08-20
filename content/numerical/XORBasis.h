/**
 * Author: me
 * Date: 2026-08-16
 * License: CC0
 * Source: me
 * Description: Incremental XOR basis (span of vectors over
 *  $\mathbb F_2$). Insert online; \texttt{x} is in the span iff it
 *  reduces to 0. Span size is $2^{|\texttt{basis}|}$.
 *  \texttt{XorBasis} is for ints; \texttt{BigBasis} for
 *  bitblocks. For solving $Ax=b$, see SolveLinearBinary.
 * Time: $O(B\cdot |basis|)$ per insert
 * Status: stress-tested
 */
#pragma once

struct XorBasis {
	vi basis;
	void add(int x) {
		rep(i,0,sz(basis)) x = min(x, x ^ basis[i]);
		if (x) basis.push_back(x);
	}
	bool inSpan(int x) {
		rep(i,0,sz(basis)) x = min(x, x ^ basis[i]);
		return x == 0;
	}
};

bool nonzero(const vector<uint64_t>& x) {
	for (auto a : x) if (a) return true;
	return false;
}
struct BigBasis {
	vector<vector<uint64_t>> basis;
	vector<uint64_t> reduce(vector<uint64_t> x) {
		rep(i,0,sz(basis)) {
			int state = 0;
			rep(j,0,sz(x)) {
				uint64_t cur = basis[i][j] ^ x[j];
				if (state == 0 && cur < x[j]) state = -1;
				if (state == 0 && cur > x[j]) state = 1;
				if (state <= 0) x[j] = cur;
			}
		}
		return x;
	}
	void add(vector<uint64_t> x) {
		x = reduce(x);
		if (nonzero(x)) basis.push_back(x);
	}
	bool equal(const BigBasis& o) {
		if (sz(o.basis) != sz(basis)) return false;
		for (auto& v : o.basis)
			if (nonzero(reduce(v))) return false;
		return true;
	}
};
