/**
 * Author: Ulf Lundstrom
 * Date: 2009-03-21
 * License: CC0
 * Source:
 * Description: Returns where \texttt{p} is as seen from \texttt{s} towards \texttt{e}. 1/0/-1 $\Leftrightarrow$ left/on line/right.
 * If the optional argument \texttt{eps} is given 0 is returned if \texttt{p} is within distance \texttt{eps} from the line.
 * Usage:
 * 	bool left = sideOf(p1,p2,q)==1;
 * Status: tested
 */
#pragma once

#include "Point.h"

int sideOf(pp s, pp e, pp p) { return sgn(orient(s, e, p)); }

int sideOf(pp s, pp e, pp p, double eps) {
	auto a = crossp(e-s, p-s);
	double l = dist(e-s)*eps;
	return (a > l) - (a < -l);
}
