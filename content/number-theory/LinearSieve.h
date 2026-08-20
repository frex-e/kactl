/**
 * Author: me
 * Date: 2026-08-16
 * License: CC0
 * Source: me
 * Description: Linear sieve. \texttt{lp[i]} is the least
 *  prime factor of $i$; \texttt{pr} lists primes $\le N$.
 *  Factor $i$ by repeatedly dividing out \texttt{lp[i]}.
 * Time: $O(N)$
 * Status: stress-tested
 */
#pragma once

const int SIEVE_N = 10000000;
vi lp, pr;
void linearSieve() {
	lp.assign(SIEVE_N + 1, 0);
	pr.clear();
	for (int i = 2; i <= SIEVE_N; ++i) {
		if (lp[i] == 0) { lp[i] = i; pr.push_back(i); }
		for (int j = 0; pr[j] <= SIEVE_N / i; ++j) {
			lp[i * pr[j]] = pr[j];
			if (pr[j] == lp[i]) break;
		}
	}
}
