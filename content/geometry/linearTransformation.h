/**
 * Author: Per Austrin, Ulf Lundstrom
 * Date: 2009-04-09
 * License: CC0
 * Source:
 * Description:
\kactlfigdesc{Apply the linear transformation (translation, rotation and scaling) which takes line \texttt{p0}-\texttt{p1} to line \texttt{q0}-\texttt{q1} to point \texttt{r}.}{content/geometry/linearTransformation}
 * Status: not tested
 */
#pragma once

#include "Point.h"

typedef Point<double> P;
P linearTransformation(P p0, P p1, P q0, P q1, P r) {
	P dp = p1-p0, dq = q1-q0, num(dp.cross(dq), dp.dot(dq));
	return q0 + P((r-p0).cross(num), (r-p0).dot(num))/dp.dist2();
}
