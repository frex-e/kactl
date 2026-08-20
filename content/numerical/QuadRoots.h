/**
 * Author: idk, me
 * Date: 2025-10-23
 * License: CC0
 * Source: https://github.com/caterpillow/cactl QuadRoots.h
 *  (originally cp-geo)
 * Description: Real roots of $ax^2+bx+c=0$ with better
 *  precision than the naive formula. Assumes $a\neq 0$.
 *  Returns the number of roots (0, 1, or 2); writes them
 *  to \texttt{out} (unordered; repeated root when one).
 * Time: $O(1)$
 * Status: stress-tested
 */
#pragma once

int quadRoots(double a, double b, double c,
		pair<double, double>& out) {
	assert(a != 0);
	double disc = b*b - 4*a*c;
	if (disc < 0) return 0;
	double sum = (b >= 0) ? -b - sqrt(disc) : -b + sqrt(disc);
	out = {sum/(2*a), sum == 0 ? 0 : (2*c)/sum};
	return 1 + (disc > 0);
}
