/**
 * Author: caterpillow
 * Date: 2026-09-09
 * License: CC0
 * Source: https://github.com/caterpillow/cactl
 *  content/data-structures/MonotonicCHT.h
 * Description: Add lines $kx+m$ in nondecreasing slope order
 *  and query maximum values at nondecreasing $x$. Additions
 *  and queries may be interleaved; query requires a line.
 *  For minimum, negate slopes, intercepts and answers
 *  (original slopes must then be nonincreasing).
 *  Answers must fit in ll; products of coefficient
 *  differences must fit in signed \texttt{\_\_int128}.
 *  LineContainer handles arbitrary slopes and queries.
 * Time: $O(1)$ amortized per operation, $O(N+Q)$ total.
 * Memory: $O(N)$
 * Status: stress-tested
 */
#pragma once

struct MonotonicCHT {
	using T = __int128;
	struct Line {
		ll k, m;
		T eval(ll x) const { return (T)k * x + m; }
	};
	vector<Line> h;
	int p = 0;
	bool bad(Line a, Line b, Line c) {
		return (T(b.k)-a.k) * (T(c.m)-b.m)
			>= (T(b.m)-a.m) * (T(c.k)-b.k);
	}
	void add(ll k, ll m) {
		Line l{k, m};
		if (sz(h) && h.back().k == k && h.back().m >= m)
			return;
		while (sz(h) && (h.back().k == k || (sz(h) > 1
			&& bad(h[sz(h)-2], h.back(), l))))
			h.pop_back();
		h.push_back(l), p = min(p, sz(h)-1);
	}
	ll query(ll x) {
		assert(sz(h));
		while (p+1 < sz(h) && h[p+1].eval(x) > h[p].eval(x))
			p++;
		return (ll)h[p].eval(x);
	}
};
