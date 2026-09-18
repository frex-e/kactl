/**
 * Author: Ulf Lundstrom
 * Date: 2009-03-21
 * License: CC0
 * Source: Basic math
 * Description:\\
\begin{minipage}{\dimexpr\linewidth-15mm\relax}
Returns the signed distance between point \texttt{p} and the line containing points \texttt{a} and \texttt{b}.
Positive value on left side and negative on right as seen from \texttt{a} towards \texttt{b}. \texttt{a==b} gives nan.
P is \texttt{complex<T>} where T is double or long long.
It uses products in intermediate steps so watch out for overflow if using \texttt{int} or \texttt{long long}.
In 3D use \texttt{(b-a).cross(p-a).dist()/(b-a).dist()}
with \texttt{Point3D}; that distance is non-negative.
\end{minipage}%
\begin{minipage}{15mm}
\includegraphics[width=\textwidth]{content/geometry/lineDistance}
\end{minipage}
 * Status: tested
 */
#pragma once

#include "Point.h"

template<class P>
double lineDist(P a, P b, P p) {
	return (double)crossp(b-a, p-a)/dist(b-a);
}
