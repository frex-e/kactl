/**
 * Author: Simon Lindholm
 * Date: 2019-04-17
 * License: CC0
 * Source: https://codeforces.com/blog/entry/58747
 * Description: Finds the closest pair of points.
 * Time: O(n \log n)
 * Status: stress-tested
 */
#pragma once

#include "Point.h"

typedef complex<ll> P;
pair<P, P> closest(vector<P> v) {
	assert(sz(v) > 1);
	set<P, PointLess> S;
	sort(all(v), [](P a, P b) { return a.imag() < b.imag(); });
	pair<ll, pair<P, P>> ret{LLONG_MAX, {P(), P()}};
	int j = 0;
	for (P p : v) {
		P d{1 + (ll)sqrt(ret.first), 0};
		while (v[j].imag() <= p.imag() - d.real()) S.erase(v[j++]);
		auto lo = S.lower_bound(p - d), hi = S.upper_bound(p + d);
		for (; lo != hi; ++lo)
			if (ll d2 = norm(*lo - p); d2 < ret.first)
				ret = {d2, {*lo, p}};
		S.insert(p);
	}
	return ret.second;
}
