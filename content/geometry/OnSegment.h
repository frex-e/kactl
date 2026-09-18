/**
 * Author: Victor Lecomte, chilli
 * Date: 2019-04-26
 * License: CC0
 * Source: https://vlecomte.github.io/cp-geo.pdf
 * Description: Returns true iff \texttt{p} lies on the line segment from \texttt{s} to \texttt{e}.
 * Use \texttt{(segDist(s,e,p)<=epsilon)} instead when using \texttt{complex<double>}.
 * Status:
 */
#pragma once

#include "Point.h"

template<class P> bool onSegment(P s, P e, P p) {
	return orient(p, s, e) == 0 && dotp(s - p, e - p) <= 0;
}
