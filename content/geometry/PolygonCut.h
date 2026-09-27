/**
 * Author: Ulf Lundstrom
 * Date: 2009-03-21
 * License: CC0
 * Source:
 * Description:\\
\begin{minipage}{\dimexpr\linewidth-15mm\relax}
Returns a vector with the vertices of a polygon with everything to the left of the line going from \texttt{s} to \texttt{e} cut away.
\end{minipage}%
\begin{minipage}{15mm}
\vspace{-6mm}
\includegraphics[width=\textwidth]{content/geometry/PolygonCut}
\vspace{-6mm}
\end{minipage}
 * Usage:
 * 	vector<pp> p = ...;
 * 	p = polygonCut(p, pp(0,0), pp(1,0));
 * Status: tested but not extensively
 */
#pragma once

#include "Point.h"

vector<pp> polygonCut(const vector<pp>& poly, pp s, pp e) {
	vector<pp> res;
	rep(i,0,sz(poly)) {
		pp cur = poly[i], prev = i ? poly[i-1] : poly.back();
		auto a = orient(s, e, cur), b = orient(s, e, prev);
		if ((a < 0) != (b < 0))
			res.push_back(cur + (prev - cur) * (a / (a - b)));
		if (a < 0)
			res.push_back(cur);
	}
	return res;
}
