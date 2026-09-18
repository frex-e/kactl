/**
 * Author: chilli, Takanori MAEHARA
 * Date: 2019-10-31
 * License: CC0
 * Source: https://github.com/spaghetti-source/algorithm/blob/master/geometry/_geom.cc#L744
 * Description: Returns the area of the intersection of a circle with a
 * ccw polygon.
 * Time: O(n)
 * Status: Tested on GNYR 2019 Gerrymandering, stress-tested
 */
#pragma once

#include "../../content/geometry/Point.h"

typedef pp P;
double angleBetween(P p, P q) {
	return atan2(crossp(p, q), dotp(p, q));
}
double circlePoly(P c, double r, vector<P> ps) {
	auto tri = [&](P p, P q) {
		auto r2 = r * r / 2;
		P d = q - p;
		if (p == q) return 0.0;
		auto a = dotp(d, p)/norm(d), b = (norm(p)-r*r)/norm(d);
		auto det = a * a - b;
		if (det <= 0) return angleBetween(p, q) * r2;
		auto s = max(0., -a-sqrt(det)), t = min(1., -a+sqrt(det));
		if (t < 0 || 1 <= s) return angleBetween(p, q) * r2;
		P u = p + d * s, v = q + d * (t-1);
		return angleBetween(p,u) * r2 + crossp(u,v)/2
		     + angleBetween(v,q) * r2;
	};
	auto sum = 0.0;
	rep(i,0,sz(ps))
		sum += tri(ps[i] - c, ps[(i + 1) % sz(ps)] - c);
	return sum;
}
