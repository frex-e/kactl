/**
 * Author: Simon Lindholm
 * Date: 2015-01-31
 * License: CC0
 * Source: me
 * Description: A class for ordering angles (as represented by complex points and
 *  a number of rotations around the origin). Useful for rotational sweeping.
 *  Sometimes also represents points or vectors.
 * Usage:
 *  vector<Angle> v = {w[0], w[0].t360() ...}; // sorted
 *  int j = 0; rep(i,0,n) { while (v[j] < v[i].t180()) ++j; }
 *  // sweeps j such that (j-i) represents the number of positively oriented triangles with vertices at 0 and i
 * Products must fit in ll. Never compare a zero direction.
 * Status: stress-tested
 */
#pragma once

#include "Point.h"

struct Angle {
	complex<ll> p;
	int t;
	Angle(complex<ll> p, int t=0) : p(p), t(t) {}
	Angle operator-(Angle b) const { return {p-b.p, t}; }
	int half() const {
		assert(p != complex<ll>());
		return p.imag() < 0 || (p.imag() == 0 && p.real() < 0);
	}
	Angle t90() const {
		return {perp(p), t + (half() && p.real() >= 0)};
	}
	Angle t180() const { return {-p, t + half()}; }
	Angle t360() const { return {p, t + 1}; }
};
bool operator<(Angle a, Angle b) {
	// Add norm(a.p), norm(b.p) to break ties by distance.
	if (a.t != b.t) return a.t < b.t;
	if (a.half() != b.half()) return a.half() < b.half();
	return crossp(a.p, b.p) > 0;
}

// Given two points, this calculates the smallest angle between
// them, i.e., the angle that covers the defined line segment.
pair<Angle, Angle> segmentAngles(Angle a, Angle b) {
	if (b < a) swap(a, b);
	return (b < a.t180() ?
	        make_pair(a, b) : make_pair(b, a.t360()));
}
Angle operator+(Angle a, Angle b) { // point a + vector b
	Angle r(a.p + b.p, a.t);
	if (a.t180() < r) r.t--;
	return r.t180() < a ? r.t360() : r;
}
Angle angleDiff(Angle a, Angle b) { // angle b - angle a
	int tu = b.t - a.t; a.t = b.t;
	return {{dotp(a.p,b.p), crossp(a.p,b.p)}, tu - (b < a)};
}
