/**
 * Author: me
 * Date: 2026-08-17
 * License: CC0
 * Source: cp-algorithms (sort-and-incremental)
 * Description: Intersection of half-planes, each the left
 *  side of directed line $s\to e$ (including the boundary).
 *  Adds a bounding box of side $2\cdot 10^9$. Returns
 *  vertices of the bounded convex polygon in order, or
 *  empty if the intersection is empty.
 * Time: $O(N\log N)$
 * Status: stress-tested
 */
#pragma once

#include "Point.h"
#include "lineIntersection.h"

typedef Point<double> P;
const double HP_EPS = 1e-9, HP_INF = 1e9;

struct HP {
	P s, e, d;
	double ang;
	HP() {}
	HP(P a, P b) : s(a), e(b), d(b - a), ang(d.angle()) {}
	bool out(P p) { return d.cross(p - s) < -HP_EPS; }
	bool operator<(HP o) const { return ang < o.ang; }
};

P hpI(HP a, HP b) {
	return lineInter(a.s, a.e, b.s, b.e).second;
}

vector<P> halfPlaneInter(vector<HP> h) {
	P box[] = {P(HP_INF, HP_INF), P(-HP_INF, HP_INF),
		P(-HP_INF, -HP_INF), P(HP_INF, -HP_INF)};
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
			fabs(L.d.cross(dq.back().d)) < HP_EPS) {
			if (L.d.dot(dq.back().d) < 0) return {};
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
	vector<P> res;
	rep(i,0,sz(dq))
		res.push_back(hpI(dq[i], dq[(i+1)%sz(dq)]));
	vector<P> out;
	for (P p : res) {
		if (out.empty() || (p - out.back()).dist() > 1e-6)
			out.push_back(p);
	}
	if (sz(out) >= 2 && (out[0] - out.back()).dist() <= 1e-6)
		out.pop_back();
	if (sz(out) < 3) return {};
	double a = 0;
	rep(i,0,sz(out))
		a += out[i].cross(out[(i+1)%sz(out)]);
	if (fabs(a) < 1e-8) return {};
	return out;
}
