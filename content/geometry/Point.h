/**
 * Author: Ulf Lundstrom, Indra Kusumah-Kasim
 * License: CC0
 * Description: Complex points: $x=\texttt{real()}$, $y=\texttt{imag()}$.
 * Use \texttt{pp} for floating geometry, \texttt{complex<ll>}
 * for exact integer predicates (products must fit in \texttt{ll}).
 * \texttt{orient(a,b,c)} is positive for a left turn.
 * \texttt{norm(p)} is squared length; \texttt{abs(p)} is length,
 * \texttt{arg(p)} is angle, \texttt{p/abs(p)} is a unit vector
 * (nonzero floating points only). Use \texttt{dist} for integer
 * length: \texttt{abs(complex<ll>)} truncates. Scalars must match
 * the coordinate type, e.g. divide \texttt{pp} by \texttt{2.0}.
 * Sort with \texttt{PointLess\{\}}; complex has no ordering.
 * Usage: pp a(3,5), rotated = a * polar(1.0, acos(-1.0)/3);
 * Status: stress-tested
 */
#pragma once

typedef double dd;
typedef complex<dd> pp;

template<class T> int sgn(T x) { return (x > 0) - (x < 0); }
template<class T> T dotp(complex<T> a, complex<T> b) {
	return a.real()*b.real() + a.imag()*b.imag();
}
template<class T> T crossp(complex<T> a, complex<T> b) {
	return a.real()*b.imag() - a.imag()*b.real();
}
template<class T>
T orient(complex<T> a, complex<T> b, complex<T> c) {
	return crossp(b-a, c-a);
}
template<class T> complex<T> perp(complex<T> p) {
	return {-p.imag(), p.real()};
}
template<class T> double dist(complex<T> p) {
	return hypot((double)p.real(), (double)p.imag());
}
struct PointLess {
	template<class T>
	bool operator()(complex<T> a, complex<T> b) const {
		return make_pair(a.real(), a.imag()) <
		       make_pair(b.real(), b.imag());
	}
};
