/**
 * Author: me
 * Date: 2026-08-16
 * License: CC0
 * Source: mine.typ
 * Description: Fills $\mu(n)$ for $n \in [0, \texttt{sz})$.
 *  See chapter text for inversion formulas.
 * Time: $O(N \log N)$
 * Status: stress-tested
 */
#pragma once

void calcMobius(vi& mobius) {
	int n = sz(mobius);
	fill(all(mobius), 0);
	if (n > 1) mobius[1] = -1;
	rep(i,1,n) if (mobius[i]) {
		mobius[i] = -mobius[i];
		for (int j = 2 * i; j < n; j += i)
			mobius[j] += mobius[i];
	}
}
