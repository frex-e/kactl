/**
 * Author: Victor Lecomte, chilli
 * Date: 2019-10-29
 * License: CC0
 * Source: https://vlecomte.github.io/cp-geo.pdf
 * Description: Projects point \texttt{p} onto line \texttt{ab}. Set \texttt{refl=true} to get reflection
 * of point \texttt{p} across line \texttt{ab} instead.
 * Status: stress-tested
 */
#pragma once

#include "Point.h"

pp lineProj(pp a, pp b, pp p, bool refl=false) {
	pp v = b - a;
	auto d = crossp(v, p-a) * (1+refl);
	return p - perp(v)*d/norm(v);
}
