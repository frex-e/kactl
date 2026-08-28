/**
 * Author: AtCoder Library, e-maxx (adapted)
 * Date: 2026-08-28
 * License: CC0
 * Source: ACL primitive_root_constexpr
 *  (atcoder/internal_math.hpp); HAC Alg. 4.79;
 *  https://cp-algorithms.com/algebra/primitive-root.html
 * Description: Multiplicative order of $a$ modulo prime $p$
 * (HAC 4.79: smallest $k>0$ with $a^k\equiv 1\pmod p$).
 * Assumes $p\nmid a$. \texttt{primitiveRoot(p)} is the
 * smallest generator of $\mathbb Z_p^\times$ (ACL).
 * If calling \texttt{ord} many times for the same $p$,
 * precompute \texttt{pfactors(p-1)} and pass it in.
 * Time: factoring $p-1$ once, then $O(\log^2 p)$ per call
 * Status: stress-tested
 */
#pragma once

#include "Factor.h"

vector<ull> pfactors(ull n) {
	auto f = factor(n);
	sort(all(f));
	f.erase(unique(all(f)), f.end());
	return f;
}
ull ord(ull a, ull p, const vector<ull>& pf) {
	a %= p;
	ull t = p - 1;
	for (ull q : pf)
		while (t % q == 0 && modpow(a, t / q, p) == 1)
			t /= q;
	return t;
}
ull ord(ull a, ull p) { return ord(a, p, pfactors(p - 1)); }
ull primitiveRoot(ull p) {
	if (p == 2) return 1;
	auto divs = pfactors(p - 1);
	for (ull g = 2; g < p; g++) {
		bool ok = 1;
		for (ull q : divs)
			if (modpow(g, (p - 1) / q, p) == 1) {
				ok = 0; break;
			}
		if (ok) return g;
	}
	return 0;
}
