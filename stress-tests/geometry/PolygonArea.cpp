#include "../utilities/template.h"

#include "../../content/geometry/PolygonArea.h"
#include "../../content/geometry/PolygonCenter.h"
#include "../../content/geometry/InsidePolygon.h"

void testTranslations() {
	vector<vector<pp>> shapes = {
		{{0,0},{1,0},{1,1},{0,1}},
		{{0,0},{6,4},{0,9}},
		{{0,0},{4,0},{4,1},{1,1},{1,4},{0,4}},
	};
	vector<double> areas = {2, 54, 14};
	vector<pp> centers = {{0.5,0.5},{2,13.0/3},
		{19.0/14,19.0/14}};
	rep(i,0,sz(shapes)) for (double scale : {1e-3,1.,1e3})
		for (pp shift : vector<pp>{{0,0},{1e8,1e8},
			{1e9,-1e9},{-1e12,1e12}}) {
			vector<pp> v;
			for (pp p : shapes[i]) v.push_back(p*scale+shift);
			// Account for rounding of translated input vertices.
			double err = 16*numeric_limits<double>::epsilon()
				* max(1.,abs(shift));
			rep(reversed,0,2) {
				double expected = areas[i]*scale*scale;
				if (reversed) expected = -expected;
				assert(abs(polygonArea2(v)-expected) <=
					1e-12*abs(expected)+err*scale*20);
				assert(abs(polygonCenter(v)-
					(centers[i]*scale+shift)) <= err+1e-12*scale);
				reverse(all(v));
			}
		}
	assert(polygonArea2({}) == 0);
	assert(polygonArea2({{1e9,1e9}}) == 0);
	assert(polygonArea2({{1e9,1e9},{1e9+1,1e9}}) == 0);
}

int main() {
	testTranslations();
	srand(0);
	typedef complex<double> P;
	vector<P> ps = {P{0,0}, P{6,4}, P{0,9}};
	int count = 0;
	P su{0,0};
	rep(it,0,100000) {
		double x = rand() / (RAND_MAX + 1.0);
		double y = rand() / (RAND_MAX + 1.0);
		x *= 10;
		y *= 10;
		if (!inPolygon(ps, P{x,y}, true)) continue;
		count++;
		su = su + P{x,y};
	}
	su = su / double(count);
	double approxArea = (double)count / 100000 * 100;
	assert(abs(polygonArea2(ps)/2.0 - approxArea) < 1);
	auto p = polygonCenter(ps);
	assert(abs(p.real() - su.real()) < 1e-1 && abs(p.imag() - su.imag()) < 1e-1);
	cout<<"Tests passed!"<<endl;
}
