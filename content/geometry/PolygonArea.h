/**
 * Author: Ulf Lundstrom
 * Date: 2009-03-21
 * License: CC0
 * Source: tinyKACTL
 * Description: Returns twice the signed area of a polygon.
 *  Clockwise enumeration gives negative area.
 * Status: Stress-tested and tested on kattis:polygonarea
 */
#pragma once

#include "Point.h"

double polygonArea2(const vector<pp>& v) {
	if (v.empty()) return 0;
	double a = crossp(v.back(), v[0]);
	rep(i,0,sz(v)-1) a += crossp(v[i], v[i+1]);
	return a;
}
