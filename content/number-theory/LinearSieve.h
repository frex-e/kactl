/**
 * Author: me
 * Date: 2026-08-16
 * License: CC0
 * Source: me
 * Description: Linear sieve. \texttt{lp[i]} is the least
 *  prime factor of $i$; \texttt{pr} lists primes $\le n$.
 *  Default $n=$ \texttt{SIEVE_N}. Larger $n$ skips \texttt{lp}
 *  (same loop, $O(n)$ bitset). Factor $i$ by dividing out
 *  \texttt{lp[i]}.
 * Time: $O(N)$
 * Status: stress-tested, Library Checker enumerate\_primes
 */
#pragma once

const int SIEVE_N = 10000000;
vi lp, pr;
void linearSieve(int n = SIEVE_N) {
	pr.clear();
	if (n <= SIEVE_N) {
		lp.assign(n + 1, 0);
		for (int i = 2; i <= n; ++i) {
			if (lp[i] == 0) { lp[i] = i; pr.push_back(i); }
			for (int j = 0; pr[j] <= n / i; ++j) {
				lp[i * pr[j]] = pr[j];
				if (pr[j] == lp[i]) break;
			}
		}
		return;
	}
	lp.clear();
	vector<char> vis(n + 1);
	for (int i = 2; i <= n; ++i) {
		if (!vis[i]) pr.push_back(i);
		for (int p : pr) {
			if (p > n / i) break;
			vis[i * p] = 1;
			if (i % p == 0) break;
		}
	}
}
