/**
 * Author: me
 * Date: 2026-08-17
 * License: CC0
 * Source: folklore
 * Description: Enumerates the $O(\sqrt n)$ blocks where
 *  $\lfloor n/i\rfloor$ is constant. Calls
 *  \texttt{f(l, r, q)} for each $1\le l\le r\le n$ with
 *  $\lfloor n/i\rfloor = q$ for all $i\in[l,r]$.
 * Time: $O(\sqrt n)$
 * Status: stress-tested
 */
#pragma once

template<class F>
void floorBlocks(ll n, F f) {
	for (ll l = 1, r; l <= n; l = r + 1) {
		ll q = n / l;
		r = n / q;
		f(l, r, q);
	}
}
