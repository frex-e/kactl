/**
 * Author: me
 * Date: 2026-08-28
 * License: CC0
 * Source: folklore
 * Description: Multiplicative order of $a$ modulo prime $p$
 * (smallest $k>0$ with $a^k\equiv 1\pmod p$).
 * Assumes $p\nmid a$. \texttt{primitiveRoot(p)} is the
 * smallest generator of $\mathbb Z_p^\times$ (or $0$ if none).
 * Time: factoring $p-1$, then $O(\log^2 p)$ per candidate
 * Status: stress-tested
 */
#pragma once

#include "Factor.h"

ull ord(ull a, ull p) {
	a %= p;
	ull n = p - 1;
	for (ull q : set<ull>(all(factor(n))))
		while (n % q == 0 && modpow(a, n / q, p) == 1)
			n /= q;
	return n;
}
ull primitiveRoot(ull p) {
	auto f = set<ull>(all(factor(p - 1)));
	for (ull g = 1; g < p; g++) {
		bool ok = 1;
		for (ull q : f)
			if (modpow(g, (p - 1) / q, p) == 1) ok = 0;
		if (ok) return g;
	}
	return 0;
}
