/**
 * Author: me
 * Date: 2026-08-17
 * License: CC0
 * Source: cp-algorithms (sort-and-incremental)
 * Description: Intersection of half-planes, each the left
 *  side of directed line $s\to e$ (including the boundary).
 *  Adds a bounding box of side $2\cdot 10^9$. Returns
 *  vertices of the bounded convex polygon in order, or
 *  empty if the intersection is empty or has zero area.
 *  Requires $s\ne e$. \texttt{HP\_EPS} is a distance
 *  tolerance.
 * Time: $O(N\log N)$
 * Status: stress-tested
 */
#pragma once

#include "Point.h"
#include "PolygonArea.h"

const double HP_EPS = 1e-9, HP_INF = 1e9;

struct HP {
	pp s, e, d;
	double ang;
	HP() {}
	HP(pp a, pp b) : s(a), e(b), d((b-a)/abs(b-a)),
		ang(arg(d)) {}
	bool out(pp p) { return crossp(d, p - s) < -HP_EPS; }
	bool operator<(HP o) const { return ang < o.ang; }
};

pp hpI(HP a, HP b) {
	return a.s + a.d * (crossp(b.s-a.s, b.d) /
		crossp(a.d, b.d));
}

vector<pp> halfPlaneInter(vector<HP> h) {
	pp box[] = {pp(HP_INF, HP_INF), pp(-HP_INF, HP_INF),
		pp(-HP_INF, -HP_INF), pp(HP_INF, -HP_INF)};
	rep(i,0,4) h.push_back(HP(box[i], box[(i+1)%4]));
	sort(all(h));
	deque<HP> dq;
	for (HP L : h) {
		while (sz(dq) > 1 &&
			L.out(hpI(dq.back(), dq[sz(dq)-2])))
			dq.pop_back();
		while (sz(dq) > 1 && L.out(hpI(dq[0], dq[1])))
			dq.pop_front();
		if (!dq.empty() &&
			(L.d == dq.back().d || L.d == -dq.back().d ||
			crossp(L.d, dq.back().d) == 0)) {
			if (dotp(L.d, dq.back().d) < 0) return {};
			if (L.out(dq.back().s)) dq.pop_back();
			else continue;
		}
		dq.push_back(L);
	}
	while (sz(dq) > 2 &&
		dq[0].out(hpI(dq.back(), dq[sz(dq)-2])))
		dq.pop_back();
	while (sz(dq) > 2 &&
		dq.back().out(hpI(dq[0], dq[1])))
		dq.pop_front();
	if (sz(dq) < 3) return {};
	vector<pp> res;
	rep(i,0,sz(dq))
		res.push_back(hpI(dq[i], dq[(i+1)%sz(dq)]));
	vector<pp> out;
	for (pp p : res) {
		if (out.empty() || abs(p - out.back()) > HP_EPS)
			out.push_back(p);
	}
	if (sz(out) >= 2 && abs(out[0]-out.back()) <= HP_EPS)
		out.pop_back();
	if (sz(out) < 3) return {};
	if (polygonArea2(out) == 0) return {};
	return out;
}
