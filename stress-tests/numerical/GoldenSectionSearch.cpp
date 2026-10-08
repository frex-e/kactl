#include "../utilities/template.h"
#include "../../content/numerical/GoldenSectionSearch.h"

double center;
int calls;
double quadratic(double x) {
	assert(++calls < 1000); // A stalled search must fail fast.
	return (x-center)*(x-center);
}

void check(double lo, double hi, double minimum) {
	center = minimum; calls = 0;
	double got = gss(lo, hi, quadratic);
	double want = clamp(minimum, lo, hi);
	double ulp = max(nextafter(want, INFINITY)-want,
		want-nextafter(want, -INFINITY));
	assert(lo <= got && got <= hi);
	assert(abs(got-want) <= 1e-7 + 4*ulp);
}

int main() {
	check(-1000, 1000, -1/.6);
	check(0, 1, 0);
	check(0, 1, 1);
	check(0, 1, -10);
	check(0, 1, 10);
	check(4, 4, 4);
	check(1e10, 1e10+1, 1e10+.5); // Old code hangs.
	check(-1e10-1, -1e10, -1e10-.5);
	check(1e10, nextafter(1e10, INFINITY), 1e10);
	check(1e16, 1e16+16, 1e16+8);
	mt19937 rng(20261008);
	uniform_real_distribution<double> unit(0, 1);
	for (double offset : {0., 1e5, -1e5, 1e10, -1e10, 1e16})
		rep(it,0,2000) {
			double width = pow(10., -6 + 12*unit(rng));
			check(offset, offset+width, offset+width*unit(rng));
		}
	cout << "Tests passed!" << endl;
}
