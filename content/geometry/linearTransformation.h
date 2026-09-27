/**
 * Author: Per Austrin, Ulf Lundstrom
 * Date: 2009-04-09
 * License: CC0
 * Source:
 * Description:\\
\begin{minipage}{\dimexpr\linewidth-15mm\relax}
Apply the linear transformation (translation, rotation and scaling) which takes line \texttt{p0}-\texttt{p1} to line \texttt{q0}-\texttt{q1} to point \texttt{r}.
\end{minipage}%
\begin{minipage}{15mm}
\vspace{-8mm}
\includegraphics[width=\textwidth]{content/geometry/linearTransformation}
\vspace{-2mm}
\end{minipage}
 * Status: not tested
 */
#pragma once

#include "Point.h"

pp linearTransformation(pp p0, pp p1, pp q0, pp q1, pp r) {
	return q0 + (r-p0) * (q1-q0) / (p1-p0);
}
