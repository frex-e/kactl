/**
 * Author: Victor Lecomte, chilli
 * Date: 2019-04-26
 * License: CC0
 * Source: https://vlecomte.github.io/cp-geo.pdf
 * Description: Returns true iff \texttt{p} lies on the line segment from \texttt{s} to \texttt{e}.
 * Use \texttt{(segDist(s,e,p)<=epsilon)} for an epsilon check.
 * Status:
 */
#pragma once

#include "Point.h"

bool onSegment(pp s, pp e, pp p) {
	return orient(p, s, e) == 0 && dotp(s - p, e - p) <= 0;
}
