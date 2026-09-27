/**
 * Author: Ulf Lundstrom, Indra Kusumah-Kasim
 * License: CC0
 * Description: Complex points: $x=\texttt{real()}$, $y=\texttt{imag()}$.
 * All planar geometry uses \texttt{pp = complex<double>}.
 * \texttt{orient(a,b,c)} is positive for a left turn.
 * \texttt{norm(p)} is squared length; \texttt{abs(p)} is length,
 * \texttt{arg(p)} is angle, \texttt{p/abs(p)} is a unit vector
 * (nonzero points only).
 * Sort with \texttt{PointLess\{\}}; complex has no ordering.
 * Usage: pp a(3,5), rotated = a * polar(1.0, acos(-1.0)/3);
 * Status: stress-tested
 */
#pragma once

typedef double dd;
typedef complex<dd> pp;

int sgn(double x) { return (x > 0) - (x < 0); }
double dotp(pp a, pp b) {
	return a.real()*b.real() + a.imag()*b.imag();
}
double crossp(pp a, pp b) {
	return a.real()*b.imag() - a.imag()*b.real();
}
double orient(pp a, pp b, pp c) {
	return crossp(b-a, c-a);
}
pp perp(pp p) {
	return {-p.imag(), p.real()};
}
double dist(pp p) { return abs(p); }
struct PointLess {
	bool operator()(pp a, pp b) const {
		return make_pair(a.real(), a.imag()) <
		       make_pair(b.real(), b.imag());
	}
};
