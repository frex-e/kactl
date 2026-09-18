#include "../utilities/template.h"
#include "../../content/geometry/Angle.h"

long double value(Angle a) {
	long double pi = acos(-1.L);
	long double v = atan2((long double)a.p.imag(), (long double)a.p.real());
	return (v < 0 ? v + 2*pi : v) + 2*pi*a.t;
}
int main() {
	mt19937 rng(34);
	auto rnd = [&]() { return (ll)(rng()%201)-100; };
	long double pi = acos(-1.L);
	rep(it,0,100000) {
		Angle a({rnd(),rnd()}, int(rng()%5)-2);
		Angle b({rnd(),rnd()}, int(rng()%5)-2);
		if (a.p == complex<ll>() || b.p == complex<ll>()) continue;
		assert((a < b) == (value(a) < value(b)));
		assert(abs(value(a.t90()) - value(a) - pi/2) < 1e-12);
		assert(abs(value(a.t180()) - value(a) - pi) < 1e-12);
		assert(abs(value(a.t360()) - value(a) - 2*pi) < 1e-12);
		assert(abs(value(angleDiff(a,b)) - (value(b)-value(a))) < 1e-12);
		if (a.p+b.p != complex<ll>()) {
			Angle sum = a+b;
			assert(sum.p == a.p+b.p);
			assert(abs(value(sum)-value(a)) <= pi + 1e-12);
		}
		a.t = b.t = 0;
		auto [s,e] = segmentAngles(a,b);
		assert(value(e)-value(s) >= -1e-12);
		assert(value(e)-value(s) <= pi+1e-12);
	}
	cout << "Tests passed!" << endl;
}
