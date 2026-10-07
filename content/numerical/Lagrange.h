/**
 * Author: OpenAI
 * Date: 2026-10-07
 * License: CC0
 * Source: folklore
 * Description: Evaluates $P(x)$ modulo a prime from samples
 * $\texttt{y[i]} = P(i)$, $i=0,\dots,n-1$, where
 * $\deg P < n \le \texttt{mod}$ and $n > 0$.
 * Requires $0 \le \texttt{y[i]} < \texttt{mod}$ and
 * products to fit in \texttt{ll}. Handles any signed
 * \texttt{x}. Template modulus defaults to $10^9+7$.
 * Useful for polynomial counting and sums of powers:
 * $\sum_{i=1}^x i^k$ has degree $k+1$ if
 * $k+1 < \texttt{mod}$, so use $k+2$ samples.
 * Prove a degree bound first.
 * Usage: lagrange({0, 1, 4}, 10) // 100
 * lagrange<998244353>(y, x)
 * Time: O(n + \log \texttt{mod})
 * Memory: O(n)
 * Status: stress-tested
 */
#pragma once

#include "../number-theory/ModPow.h"

template<ll mod = 1000000007>
ll lagrange(const vector<ll>& y, ll x) {
	int n = sz(y);
	assert(0 < n && n <= mod);
	x %= mod;
	if (x < 0) x += mod;
	if (x < n) return y[x];
	vector<ll> pre(n+1, 1), suf(pre), ifac(n);
	ll fac = 1, ans = 0;
	rep(i,0,n) {
		pre[i+1] = pre[i] * (x-i) % mod;
		if (i) fac = fac * i % mod;
	}
	ifac[n-1] = modpow<mod>(fac, mod-2);
	for (int i = n-1; i > 0; --i)
		ifac[i-1] = ifac[i] * i % mod;
	for (int i = n-1; i >= 0; --i)
		suf[i] = suf[i+1] * (x-i) % mod;
	rep(i,0,n) {
		ll t = y[i] * pre[i] % mod * suf[i+1] % mod;
		t = t * ifac[i] % mod * ifac[n-1-i] % mod;
		if ((n-1-i) & 1) t = (mod-t) % mod;
		ans = (ans+t) % mod;
	}
	return ans;
}
