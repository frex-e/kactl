#include "../utilities/template.h"

#include "../../content/geometry/CirclePolygonIntersection.h"
#include "../utilities/genPolygon.h"

namespace orig{
typedef complex<long double> P;
long double areaCT(P pa, P pb, long double r) {
	if (abs(pa) < abs(pb)) swap(pa, pb);
	if (sgn(abs(pb)) == 0) return 0;
	long double a = abs(pb), b = abs(pa), c = abs(pb - pa);
	long double sinB = fabs(crossp(pb, pb - pa) / a / c), cosB = dotp(pb, pb - pa) / a / c,
		sinC = fabs(crossp(pa, pb) / a / b), cosC = dotp(pa, pb) / a / b;
	long double B = atan2(sinB, cosB), C = atan2(sinC, cosC);
	// Direct altitude avoids rounding a tangent asin argument below 1.
	long double h = fabs(crossp(pa, pb)) / c;
	if (a > r) {
		long double S = C / 2 * r * r;
		if (h < r && B < M_PI / 2)
			S -= (acos(h / r) * r * r - h * sqrt(r * r - h * h));
		return S;
	} else if (b > r) {
		long double theta = M_PI - B - asin(min(1.L, h / r));
		return a * r * sin(theta) / 2 + (C - theta) / 2 * r * r;
	} else return sinC * a * b / 2;
}
long double circlePoly(P c, long double r, vector<P> poly) {
	long double area = 0;
	rep(i,0,sz(poly)){
		auto a = poly[i] - c, b = poly[(i+1)%sz(poly)] - c;
		area += areaCT(a, b, r) * sgn(crossp(a, b));
	}
	return area;
}
}

signed main() {
	ios::sync_with_stdio(0);
	cin.tie(0);
	const int lim=5;
	for (int i=0;i<100000; i++) {

		vector<complex<int>> pts;
		for (int j=0; j<10; j++) {
			int x = rand()%lim, y = rand()%lim;
			pts.push_back(complex<int>(x, y));
		}

		auto polyInt = genPolygon(pts);

		int cx = rand()%lim, cy = rand()%lim;
		auto c = P(cx, cy);
		auto c2 = orig::P(cx, cy);
		double r= rand()%(2*lim);

		vector<P> poly;
		vector<orig::P> poly2;
		for (auto j: polyInt) {
			poly.push_back(P(j.real(), j.imag()));
			poly2.push_back(orig::P(j.real(), j.imag()));
		}
		auto res1 = circlePoly(c, r, poly);
		auto res2 = orig::circlePoly(c2, r, poly2);

		if (abs(res1 - res2) > 1e-8) {
			cout<<abs(res1-res2)<<' '<<res1<<' '<<res2<<endl;
			assert(false);
		}
	}
	cout<<"Tests passed!"<<endl;
}
