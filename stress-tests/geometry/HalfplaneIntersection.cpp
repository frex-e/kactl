#include "../utilities/template.h"

#include "../../content/geometry/PolygonArea.h"
#include "../../content/geometry/PolygonCut.h"
#include "../../content/geometry/InsidePolygon.h"
#include "../../content/geometry/HalfplaneIntersection.h"

typedef Point<double> P;

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
			if (a.dist2() == b.dist2() && a == b) continue;
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
			if (p.x <= -B+1e-6 || p.x >= B-1e-6 ||
				p.y <= -B+1e-6 || p.y >= B-1e-6) continue;
			bool inH = insideAll(h, p);
			if (!inH) {
				if (!res.empty())
					assert(!inPolygon(res, p, true));
			} else if (!res.empty()) {
				bool strictOut = false;
				for (HP L : h)
					if (L.d.cross(p - L.s) < 1e-6)
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
	testVsCutAndSample();
	cout << "Tests passed!" << endl;
}
