/**
 * Author: Ulf Lundstrom
 * Date: 2009-04-08
 * License: CC0
 * Source:
 * Description: Returns the center of mass for a polygon
 *  with nonzero signed area. Uses local coordinates to
 *  avoid cancellation when far from the origin.
 * Time: O(n)
 * Status: stress-tested
 */
#pragma once

#include "Point.h"

pp polygonCenter(const vector<pp>& v) {
	pp res(0, 0); double A = 0;
	rep(i,1,sz(v)-1) {
		pp p = v[i]-v[0], q = v[i+1]-v[0];
		double a = crossp(p, q);
		res += (p + q) * a;
		A += a;
	}
	return v[0] + res / A / 3.0;
}
