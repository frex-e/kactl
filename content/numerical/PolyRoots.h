/**
 * Author: Per Austrin
 * Date: 2004-02-08
 * License: CC0
 * Description: Sorted distinct real roots of a nonzero
 *  polynomial, including repeated roots. Floating point:
 *  very close roots may merge. Coefficients, bounds and
 *  intermediate evaluations must be finite; avoid underflow.
 *  xmin/xmax should enclose all roots. Search sentinels
 *  are xmin-1 and xmax+1; returned roots are not clipped
 *  to $[xmin,xmax]$. A linear polynomial ignores bounds.
 * Usage: polyRoots({{2,-3,1}},-1e9,1e9) // solve x^2-3x+2 = 0
 * Time: O(n^3 \log(1/\epsilon))
 * Status: stress-tested
 */
#pragma once

#include "Polynomial.h"

vector<double> polyRoots(Poly p, double xmin, double xmax) {
	while (!p.a.empty() && p.a.back() == 0) p.a.pop_back();
	if (sz(p.a) < 2) return {};
	if (sz(p.a) == 2) { return {-p.a[0]/p.a[1]}; }
	// Compensated Horner: distinguish tiny extrema from zeros.
	auto eval = [&](double x) {
		double v = 0, err = 0, scale = 0;
		for (int i = sz(p.a); i--;) {
			double a = p.a[i], prod = v*x, sum = prod+a;
			double z = sum-prod;
			err = err*x + (fma(v,x,-prod) +
				((prod-(sum-z)) + (a-z)));
			v = sum; scale = scale*abs(x) + abs(a);
		}
		double eps = numeric_limits<double>::epsilon();
		v += err;
		return pair(v, abs(v) <=
			64.0 * sz(p.a) * sz(p.a) * eps * eps * scale);
	};
	vector<double> ret;
	Poly der = p;
	der.diff();
	auto dr = polyRoots(der, xmin, xmax);
	dr.push_back(xmin-1);
	dr.push_back(xmax+1);
	sort(all(dr));
	dr.erase(unique(all(dr)), dr.end());
	rep(i,0,sz(dr)) {
		double l = dr[i];
		auto [fl, zl] = eval(l);
		if (zl) ret.push_back(l);
		if (i+1 == sz(dr)) break;
		double h = dr[i+1];
		auto [fh, zh] = eval(h);
		if (zl || zh || (fl > 0) == (fh > 0)) continue;
		for (;;) {
			double m = l/2 + h/2, f = eval(m).first;
			if (m == l || m == h) break;
			if (f == 0) { l = h = m; break; }
			if ((f > 0) == (fl > 0)) l = m;
			else h = m;
		}
		ret.push_back(l/2 + h/2);
	}
	return ret;
}
