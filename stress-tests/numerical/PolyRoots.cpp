#include "../utilities/template.h"
#include "../../content/numerical/PolyRoots.h"

void check(vector<double> a, vector<double> want,
		double lo = -10, double hi = 10) {
	sort(all(want));
	want.erase(unique(all(want)), want.end());
	auto got = polyRoots({a}, lo, hi);
	if (sz(got) != sz(want)) {
		cerr << "Coefficients:";
		for (double x : a) cerr << ' ' << x;
		cerr << "\nRoots:";
		for (double x : got) cerr << ' ' << x;
		cerr << "\nExpected:";
		for (double x : want) cerr << ' ' << x;
		cerr << endl;
	}
	assert(sz(got) == sz(want));
	rep(i,0,sz(got))
		assert(abs(got[i]-want[i]) < 1e-8*(1+abs(want[i])));
}

vector<double> fromRoots(const vector<double>& roots) {
	vector<double> a{1};
	for (double r : roots) {
		vector<double> b(sz(a)+1);
		rep(i,0,sz(a)) b[i] -= r*a[i], b[i+1] += a[i];
		a.swap(b);
	}
	return a;
}

int main() {
	check({-1,2,-1}, {1});
	check({1,-2,1}, {1});
	check({0,0,-1}, {0});
	check({1}, {});
	check({1,0,0}, {}); // trailing zero coefficients
	check({-2,1}, {2}, 0, 1); // linear ignores bounds
	check({-1,2,-1}, {1}, -1e9, 1e9);
	check({4,0,-4,0,1}, {-sqrt(2.),sqrt(2.)});
	check({-4,0,4,0,-1}, {-sqrt(2.),sqrt(2.)});
	// Positive minima must not become spurious repeated roots.
	check({1+1e-12,-2,1}, {});
	check({1+1e-15,-2,1}, {});
	check({1e-100,0,1}, {});
	check({1-1e-12,-2,1},
		{1-sqrt(1-(1-1e-12)),1+sqrt(1-(1-1e-12))});
	for (double scale : {1.,-1.,1e-100,-1e-100,1e100}) {
		check({scale,-2*scale,scale}, {1});
		check({scale*1e-10,0,scale}, {});
	}
	mt19937 rng(20261008);
	rep(it,0,5000) {
		vector<double> roots;
		int n = 1 + (int)(rng()%9);
		rep(i,0,n) roots.push_back((int)(rng()%11)-5);
		auto a = fromRoots(roots);
		for (double& x : a) x *= it & 1 ? -1 : 1;
		check(a, roots);
	}
	// Simple and repeated dyadic roots between integers.
	check(fromRoots({-.75,-.75,.25,.5,.5}), {-.75,.25,.5});
	cout << "Tests passed!" << endl;
}
