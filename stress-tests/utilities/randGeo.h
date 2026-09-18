#pragma once

#include "../../content/geometry/Point.h"

template <class T=double>
complex<T> randIntPt(int lim) {
	return complex<T>(rand()%(lim*2) - lim, rand()%(lim*2)-lim);
}
