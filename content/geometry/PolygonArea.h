/**
 * Author: Ulf Lundstrom
 * Date: 2009-03-21
 * License: CC0
 * Source: tinyKACTL
 * Description: Returns twice the signed area of a polygon.
 *  Clockwise enumeration gives negative area.
 *  Uses coordinates relative to the first vertex.
 * Status: Stress-tested and tested on kattis:polygonarea
 */
#pragma once

#include "Point.h"

double polygonArea2(const vector<pp>& v) {
	if (v.empty()) return 0;
	double a = 0;
	rep(i,1,sz(v)-1) a += crossp(v[i]-v[0], v[i+1]-v[0]);
	return a;
}
