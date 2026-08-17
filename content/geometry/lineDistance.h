/**
 * Author: Ulf Lundstrom
 * Date: 2009-03-21
 * License: CC0
 * Source: Basic math
 * Description:\\
\begin{minipage}{75mm}
Returns the signed distance between point \texttt{p} and the line containing points \texttt{a} and \texttt{b}.
Positive value on left side and negative on right as seen from \texttt{a} towards \texttt{b}. \texttt{a==b} gives nan.
P is supposed to be \texttt{Point<T>} or \texttt{Point3D<T>} where \texttt{T} is e.g. \texttt{double} or \texttt{long long}.
It uses products in intermediate steps so watch out for overflow if using \texttt{int} or \texttt{long long}.
Using \texttt{Point3D} will always give a non-negative distance. For \texttt{Point3D}, call \texttt{.dist} on the result of the cross product.
\end{minipage}
\begin{minipage}{15mm}
\includegraphics[width=\textwidth]{content/geometry/lineDistance}
\end{minipage}
 * Status: tested
 */
#pragma once

#include "Point.h"

template<class P>
double lineDist(P a, P b, P p) {
	return (double)(b-a).cross(p-a)/(b-a).dist();
}
