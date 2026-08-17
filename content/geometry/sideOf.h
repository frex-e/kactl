/**
 * Author: Ulf Lundstrom
 * Date: 2009-03-21
 * License: CC0
 * Source:
 * Description: Returns where \texttt{p} is as seen from \texttt{s} towards \texttt{e}. 1/0/-1 $\Leftrightarrow$ left/on line/right.
 * If the optional argument \texttt{eps} is given 0 is returned if \texttt{p} is within distance \texttt{eps} from the line.
 * \texttt{P} is supposed to be \texttt{Point<T>} where \texttt{T} is e.g. \texttt{double} or \texttt{long long}.
 * It uses products in intermediate steps so watch out for overflow if using int or long long.
 * Usage:
 * 	bool left = sideOf(p1,p2,q)==1;
 * Status: tested
 */
#pragma once

#include "Point.h"

template<class P>
int sideOf(P s, P e, P p) { return sgn(s.cross(e, p)); }

template<class P>
int sideOf(P s, P e, P p, double eps) {
	auto a = (e-s).cross(p-s);
	double l = (e-s).dist()*eps;
	return (a > l) - (a < -l);
}
