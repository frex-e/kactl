/**
 * Author: Oleksandr Bacherikov, chilli
 * Date: 2019-05-05
 * License: Boost Software License
 * Source: https://codeforces.com/blog/entry/48868
 * Description: Returns the two points with max distance on a convex hull (ccw,
 * no duplicate/collinear points).
 * Status: stress-tested, tested on kattis:roberthood
 * Time: O(n)
 */
#pragma once
#include "Point.h"

template<class P>
array<P, 2> hullDiameter(vector<P> S) {
	assert(!S.empty());
	int n = sz(S), j = n < 2 ? 0 : 1;
	auto best = norm(S[0]-S[0]);
	array<P, 2> res{S[0], S[0]};
	rep(i,0,j)
		for (;; j = (j + 1) % n) {
			if (auto d2 = norm(S[i] - S[j]); d2 > best)
				best = d2, res = {S[i], S[j]};
			if (crossp(S[(j + 1) % n] - S[j], S[i + 1] - S[i]) >= 0)
				break;
		}
	return res;
}
