/**
 * Author: Victor Lecomte, chilli
 * Date: 2019-05-05
 * License: CC0
 * Source: https://vlecomte.github.io/cp-geo.pdf
 * Description:
\kactlfigdesc{If a unique intersection point of the lines going through \texttt{s1,e1} and \texttt{s2,e2} exists \texttt{\{1, point\}} is returned.
If no intersection point exists \texttt{\{0, (0,0)\}} is returned and if infinitely many exists \texttt{\{-1, (0,0)\}} is returned.
The wrong position will be returned if \texttt{P} is \texttt{Point<ll>} and the intersection point does not have integer coordinates.
Products of three coordinates are used in intermediate steps so watch out for overflow if using \texttt{int} or \texttt{ll}.}{content/geometry/lineIntersection}
 * Usage:
 * 	auto res = lineInter(s1,e1,s2,e2);
 * 	if (res.first == 1)
 * 		cout << "intersection point at " << res.second << endl;
 * Status: stress-tested, and tested through half-plane tests
 */
#pragma once

#include "Point.h"

template<class P>
pair<int, P> lineInter(P s1, P e1, P s2, P e2) {
	auto d = (e1 - s1).cross(e2 - s2);
	if (d == 0) // if parallel
		return {-(s1.cross(e1, s2) == 0), P(0, 0)};
	auto p = s2.cross(e1, e2), q = s2.cross(e2, s1);
	return {1, (s1 * p + e1 * q) / d};
}
