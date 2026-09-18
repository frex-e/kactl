/**
 * Author: Ulf Lundstrom
 * Date: 2009-04-11
 * License: CC0
 * Source: http://en.wikipedia.org/wiki/Circumcircle
 * Description:\\
\begin{minipage}{\dimexpr\linewidth-15mm\relax}
The circumcirle of a triangle is the circle intersecting all three vertices. \texttt{ccRadius} returns the radius of the circle going through points A, B and C and \texttt{ccCenter} returns the center of the same circle.
\end{minipage}%
\begin{minipage}{15mm}
\vspace{-2mm}
\includegraphics[width=\textwidth]{content/geometry/circumcircle}
\end{minipage}
 * Status: tested
 */
#pragma once

#include "Point.h"

typedef pp P;
double ccRadius(P A, P B, P C) {
	return abs(B-A)*abs(C-B)*abs(A-C)/
			abs(crossp(B-A, C-A))/2.0;
}
P ccCenter(P A, P B, P C) {
	P b = C-A, c = B-A;
	return A + perp(b*norm(c)-c*norm(b))/crossp(b, c)/2.0;
}
