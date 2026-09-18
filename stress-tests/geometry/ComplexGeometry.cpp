#include "../utilities/template.h"
#include "../../content/geometry/Point.h"
#include "../../content/geometry/ConvexHull.h"
#include "../../content/geometry/HullDiameter.h"
#include "../../content/geometry/PointInsideHull.h"
#include "../../content/geometry/PolygonArea.h"
#include "../../content/geometry/linearTransformation.h"
#include "../../content/geometry/LineProjectionReflection.h"
#include "../../content/geometry/CirclePolygonIntersection.h"
#include "../../content/geometry/HalfplaneIntersection.h"
#include "../../content/geometry/MinimumEnclosingCircle.h"
#include "../../content/geometry/SegmentIntersection.h"

int main() {
	using IP = complex<ll>;
	static_assert(is_same_v<pp, complex<double>>);
	static_assert(is_same_v<decltype(norm(IP())), ll>);
	// Exact predicates and squared distances beyond double's integer range.
	IP a(1000000000, 999999999), b(999999999, 999999998);
	assert(crossp(a, b) == -1);
	assert(norm(a) == 1999999998000000001LL);
	assert(abs(dist(IP(1,1)) - sqrt(2.0)) < 1e-15);
	assert(orient(IP(), a, b) == -1);
	assert(orient(pp(), pp(1,0), pp(0,1)) == 1);
	assert(convexHull(vector<pp>{}).empty());
	assert(!inHull(vector<pp>{}, pp(), false));
	assert(polygonArea2(vector<pp>{}) == 0);
	assert(mec({}).second == 0);
	for (vector<pp> v : {vector<pp>{{2,3}}, {{2,3},{2,3}},
		{{2,0},{0,0},{1,0},{0,0}}}) {
		auto h = convexHull(v);
		assert(h.size() <= 2);
		for (pp p : v) assert(inHull(h, p, false));
	}
	vector<pp> square{{0,0},{2,0},{2,2},{0,2}};
	assert(polygonArea2(square) == 8);
	assert(circlePoly(pp(1,1), 10, square) == 4);
	square.insert(square.begin(), square[0]);
	assert(circlePoly(pp(1,1), 10, square) == 4);
	// std::arg must remain usable after circle-polygon and half-plane includes.
	assert(abs(arg(pp(0,1)) - acos(-1.0)/2) < 1e-15);
	assert(segInter(pp(0,0), pp(2,2), pp(0,2), pp(2,0)) ==
	       vector<pp>{pp(1,1)});
	assert(segInter(IP(0,0), IP(3,0), IP(1,0), IP(4,0)) ==
	       (vector<IP>{IP(1,0), IP(3,0)}));
	assert(lineProj(IP(0,0), IP(2,0), IP(1,3)) == IP(1,0));
	assert(lineProj(IP(0,0), IP(2,0), IP(1,3), true) == IP(1,-3));
	mt19937 rng(1729);
	auto rnd = [&]() { return int(rng()%201)-100; };
	rep(it,0,10000) {
		pp p(rnd()/4.0, rnd()/4.0), q(rnd()/4.0, rnd()/4.0);
		assert(abs(dotp(p,q) - (conj(p)*q).real()) < 1e-10);
		assert(abs(crossp(p,q) - (conj(p)*q).imag()) < 1e-10);
		assert(perp(p) == p*pp(0,1));
		pp rot = polar(1.0, rnd()/7.0);
		assert(abs(abs(p*rot)-abs(p)) < 1e-10);
		if (p != q) {
			pp u(rnd(),rnd()), shift(rnd(),rnd()), scale(rnd(),rnd());
			pp got = linearTransformation(p,q,p*scale+shift,
				q*scale+shift,u);
			assert(abs(got-(u*scale+shift)) < 1e-8);
		}
		vector<pp> pts;
		rep(i,0,20) pts.emplace_back(rnd()/4.0,rnd()/4.0);
		auto h = convexHull(pts);
		auto ends = hullDiameter(h);
		double best = 0;
		for (pp x : pts) for (pp y : pts) best = max(best,norm(x-y));
		assert(norm(ends[0]-ends[1]) == best);
		assert(polygonArea2(h) > 0);
		for (pp x : pts) assert(inHull(h, x, false));
		for (int i = 0; i < sz(h); ++i)
			assert(orient(h[i],h[(i+1)%sz(h)],h[(i+2)%sz(h)]) > 0);
	}
	cout << "Tests passed!" << endl;
}
