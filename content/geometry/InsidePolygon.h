/**
 * Author: Victor Lecomte, chilli
 * Date: 2019-04-26
 * License: CC0
 * Source: https://vlecomte.github.io/cp-geo.pdf
 * Description: Returns true if \texttt{p} lies within the polygon. If \texttt{strict} is true,
 * it returns false for points on the boundary.
 * Time: O(n)
 * Usage:
 * vector<pp> v = {pp{4,4}, pp{1,2}, pp{2,1}};
 * bool in = inPolygon(v, pp{3, 3}, false);
 * Status: stress-tested and tested on kattis:pointinpolygon
 */
#pragma once

#include "Point.h"
#include "OnSegment.h"
#include "SegmentDistance.h"

bool inPolygon(vector<pp> &p, pp a, bool strict = true) {
	int cnt = 0, n = sz(p);
	rep(i,0,n) {
		pp q = p[(i + 1) % n];
		if (onSegment(p[i], q, a)) return !strict;
		//or: if (segDist(p[i], q, a) <= eps) return !strict;
		int above = (a.imag()<p[i].imag()) - (a.imag()<q.imag());
		cnt ^= above * orient(a, p[i], q) > 0;
	}
	return cnt;
}
