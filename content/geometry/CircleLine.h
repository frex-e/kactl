/**
 * Author: Victor Lecomte, chilli
 * Date: 2019-10-29
 * License: CC0
 * Source: https://vlecomte.github.io/cp-geo.pdf
 * Description: Finds the intersection between a circle and a line.
 * Returns a vector of either 0, 1, or 2 intersection points.
 * P is intended to be \texttt{complex<double>}.
 * Status: unit tested
 */

#pragma once

#include "Point.h"

template<class P>
vector<P> circleLine(P c, double r, P a, P b) {
	P ab = b - a, p = a + ab * dotp(c-a, ab) / norm(ab);
	double s = orient(a, b, c), h2 = r*r - s*s / norm(ab);
	if (h2 < 0) return {};
	if (h2 == 0) return {p};
	P h = ab / abs(ab) * sqrt(h2);
	return {p - h, p + h};
}
