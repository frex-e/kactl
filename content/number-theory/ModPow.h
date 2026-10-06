/**
 * Author: Noam527
 * Date: 2019-04-24
 * License: CC0
 * Source: folklore
 * Description: Computes $b^e \bmod m$ for $0 \le b < m$
 * and $e \ge 0$. Requires $m > 1$ and products to fit
 * in \texttt{ll}.
 * Compile-time modulus defaults to $10^9+7$.
 * Usage: modpow(b, e) // default modulus
 * modpow<998244353>(b, e) // compile-time modulus
 * modpow(b, e, p) // runtime modulus
 * Time: O(\log e)
 * Status: tested
 */
#pragma once

ll modpow(ll b, ll e, ll mod) {
	ll ans = 1;
	for (; e; b = b * b % mod, e /= 2)
		if (e & 1) ans = ans * b % mod;
	return ans;
}

template<ll mod = 1000000007>
ll modpow(ll b, ll e) { return modpow(b, e, mod); }
