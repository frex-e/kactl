#include "../utilities/template.h"

#include "../../content/geometry/PolygonArea.h"
#include "../../content/geometry/PolygonCut.h"
#include "../../content/geometry/InsidePolygon.h"
#include "../../content/geometry/HalfplaneIntersection.h"

typedef complex<double> P;

bool insideAll(const vector<HP>& h, P p) {
	for (HP L : h) if (L.out(p)) return false;
	return true;
}

void checkPoly(const vector<HP>& h, vector<P> res) {
	for (P p : res) assert(insideAll(h, p));
	if (res.empty()) return;
	double a = polygonArea2(res);
	assert(fabs(a) > 1e-6);
}

void testUnitSquare() {
	vector<HP> h = {
		HP(P(0,0), P(1,0)),
		HP(P(1,0), P(1,1)),
		HP(P(1,1), P(0,1)),
		HP(P(0,1), P(0,0)),
	};
	auto res = halfPlaneInter(h);
	assert(sz(res) >= 4);
	assert(fabs(fabs(polygonArea2(res)/2) - 1) < 1e-6);
	checkPoly(h, res);
}

void testEmpty() {
	vector<HP> h = {
		HP(P(1,0), P(1,-1)), // x >= 1
		HP(P(0,0), P(0,1)),  // x <= 0
	};
	assert(halfPlaneInter(h).empty());
}

void testPrecision() {
	mt19937 rng(314159);
	for (double side : {1e-7,1e-5,1.,1000.})
		for (pp shift : vector<pp>{{0,0},{1e8,-1e8}})
			for (double length : {1e-6,1.,1e6}) {
				vector<pp> v = {shift,shift+pp(side,0),
					shift+pp(side,side),shift+pp(0,side)};
				vector<HP> h;
				rep(i,0,4) {
					pp d = (v[(i+1)%4]-v[i])/side;
					h.emplace_back(v[i],v[i]+d*length);
				}
				double tol = 8*numeric_limits<double>::epsilon()
					* max(1.,abs(shift))+1e-12*side;
				rep(it,0,10) {
					shuffle(all(h),rng);
					vector<pp> res = halfPlaneInter(h);
					assert(sz(res) == 4);
					for (pp p : v) {
						double best = numeric_limits<double>::infinity();
						for (pp q : res) best = min(best,abs(p-q));
						assert(best <= tol);
					}
				}
			}
	for (double length : {1e-6,1.,1e6}) {
		HP h({0,0},{length,0});
		assert(h.out({0,-1e-3}));
		assert(!h.out({0,1e-3}));
	}
	rep(it,0,1000) {
		pp rot = polar(1.,double(rng())/rng.max()*6.28);
		pp shift = it%2 ? pp(1e8,-1e8) : pp();
		vector<pp> v;
		for (pp p : vector<pp>{{0,0},{1,0},{1,1},{0,1}})
			v.push_back(p*rot+shift);
		vector<HP> h;
		rep(i,0,4) {
			h.emplace_back(v[i],v[(i+1)%4]);
			h.push_back(h.back());
		}
		shuffle(all(h),rng);
		auto res = halfPlaneInter(h);
		assert(sz(res) == 4);
		assert(abs(polygonArea2(res)-polygonArea2(v)) < 1e-6);
	}
	// Near-parallel lines meet inside the bounding box.
	vector<HP> h = {HP({0,0},{1,0}),
		HP({0,0},{1,5e-10}),HP({0,0.05},{-1,0.05}),
		HP({0,0},{0,-1})};
	auto res = halfPlaneInter(h);
	assert(sz(res) == 3);
	assert(abs(polygonArea2(res)-5e6) < 1e-6);
	checkPoly(h,res);
	// Redundant parallel lines and lower-dimensional results.
	h = {HP({0,0},{1,0}),HP({0,-1},{2,-1}),
		HP({1,0},{1,1}),HP({1,1},{0,1}),
		HP({0,1},{0,0})};
	assert(abs(polygonArea2(halfPlaneInter(h))-2) < 1e-12);
	h.push_back(HP({0,0},{-1,0}));
	assert(halfPlaneInter(h).empty());
}

void testVsCutAndSample() {
	rep(it,0,200) {
		const double B = 10;
		vector<HP> h = {
			HP(P(-B,-B), P(B,-B)),
			HP(P(B,-B), P(B,B)),
			HP(P(B,B), P(-B,B)),
			HP(P(-B,B), P(-B,-B)),
		};
		int extra = rand() % 8;
		rep(i,0,extra) {
			P a(rand()%21 - 10, rand()%21 - 10);
			P b(rand()%21 - 10, rand()%21 - 10);
			if (norm(a) == norm(b) && a == b) continue;
			if (a == b) continue;
			h.push_back(HP(a, b));
		}
		auto res = halfPlaneInter(h);
		checkPoly(h, res);

		vector<P> cut = {P(-B,-B), P(B,-B), P(B,B), P(-B,B)};
		rep(i,4,sz(h))
			cut = polygonCut(cut, h[i].e, h[i].s);
		double aH = res.empty() ? 0 : fabs(polygonArea2(res)/2);
		double aC = cut.empty() ? 0 : fabs(polygonArea2(cut)/2);
		if (aH < 1e-6) aH = 0;
		if (aC < 1e-6) aC = 0;
		assert(fabs(aH - aC) < 1e-2);

		rep(s,0,3000) {
			P p(rand()%21 - 10 + (rand()%100)/100.0,
				rand()%21 - 10 + (rand()%100)/100.0);
			if (p.real() <= -B+1e-6 || p.real() >= B-1e-6 ||
				p.imag() <= -B+1e-6 || p.imag() >= B-1e-6) continue;
			bool inH = insideAll(h, p);
			if (!inH) {
				if (!res.empty())
					assert(!inPolygon(res, p, true));
			} else if (!res.empty()) {
				bool strictOut = false;
				for (HP L : h)
					if (crossp(L.d, p - L.s) < 1e-6)
						strictOut = true;
				if (!strictOut)
					assert(inPolygon(res, p, true));
			}
		}
	}
}

int main() {
	testUnitSquare();
	testEmpty();
	testPrecision();
	testVsCutAndSample();
	cout << "Tests passed!" << endl;
}
