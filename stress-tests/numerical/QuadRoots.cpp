#include "../utilities/template.h"

#include "../../content/numerical/QuadRoots.h"

double eval(double a, double b, double c, double x) {
	return (a*x + b)*x + c;
}

void checkRoots(double a, double b, double c, int n,
		pair<double, double> out) {
	assert(n == 0 || n == 1 || n == 2);
	if (n == 0) {
		assert(b*b - 4*a*c < 0);
		return;
	}
	vector<double> roots = {out.first};
	if (n == 2) roots.push_back(out.second);
	for (double x : roots) {
		double fx = eval(a, b, c, x);
		double scale = max({1.0, abs(a)*x*x, abs(b)*x, abs(c)});
		assert(abs(fx) <= 1e-9 * scale);
	}
	if (n == 1) {
		assert(abs(out.first - out.second) <= 1e-9 * max(1.0, abs(out.first)));
	} else {
		// Vieta: sum = -b/a, product = c/a
		double s = out.first + out.second;
		double p = out.first * out.second;
		assert(abs(s + b/a) <= 1e-9 * max(1.0, abs(b/a)));
		assert(abs(p - c/a) <= 1e-9 * max(1.0, abs(c/a)));
	}
}

int main() {
	// Known cases
	pair<double, double> out;
	assert(quadRoots(1, 0, 1, out) == 0); // x^2 + 1
	assert(quadRoots(1, -2, 1, out) == 1); // (x-1)^2
	checkRoots(1, -2, 1, 1, out);
	assert(quadRoots(1, -3, 2, out) == 2); // (x-1)(x-2)
	checkRoots(1, -3, 2, 2, out);

	// Cancellation: naive (-b±sqrt)/(2a) loses the small root
	int n = quadRoots(1, 1e9, 1, out);
	assert(n == 2);
	checkRoots(1, 1e9, 1, n, out);
	double small = min(abs(out.first), abs(out.second));
	assert(abs(small - 1e-9) < 1e-15); // true small root ~ 1e-9

	mt19937 rng(14);
	uniform_real_distribution<double> coef(-10, 10);
	rep(it,0,100000) {
		double a = coef(rng);
		if (abs(a) < 1e-3) a = (a < 0 ? -1 : 1);
		double b = coef(rng), c = coef(rng);
		n = quadRoots(a, b, c, out);
		checkRoots(a, b, c, n, out);
	}

	// Large coefficients
	uniform_real_distribution<double> big(-1e6, 1e6);
	rep(it,0,10000) {
		double a = big(rng);
		if (abs(a) < 1) a = (a < 0 ? -1 : 1) * 1e3;
		double b = big(rng), c = big(rng);
		n = quadRoots(a, b, c, out);
		checkRoots(a, b, c, n, out);
	}

	cout << "Tests passed!" << endl;
}
