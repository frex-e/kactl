/**
 * Author: Alex Li
 * Date: 2026-09-27
 * License: GPL-2.0
 * Source: https://algorithms.alexli.ca/closest-pair/
 * Description: Closest pair by divide and conquer.
 *  Requires at least two finite points; squared distances must
 *  not overflow or underflow. Supports fractional coordinates.
 * Time: O(n \log n)
 * Memory: O(n)
 * Status: stress-tested
 */
#pragma once

#include "Point.h"

pair<pp, pp> closest(vector<pp> v) {
	assert(sz(v) > 1);
	sort(all(v), PointLess{});
	vector<pp> tmp(sz(v));
	double best = INFINITY;
	pair<pp, pp> ret{v[0], v[1]};
	auto upd = [&](pp a, pp b) {
		if (double d = norm(a - b); d < best)
			best = d, ret = {a, b};
	};
	auto byY = [](pp a, pp b) { return a.imag() < b.imag(); };
	auto rec = [&](auto&& self, int l, int r) -> void {
		if (best == 0) return;
		if (r - l <= 3) {
			rep(i,l,r) rep(j,i+1,r) upd(v[i], v[j]);
			sort(v.begin() + l, v.begin() + r, byY);
			return;
		}
		int m = (l + r) / 2;
		double x = v[m].real();
		self(self, l, m), self(self, m, r);
		if (best == 0) return;
		auto a = v.begin() + l, b = v.begin() + m;
		auto c = v.begin() + r;
		merge(a, b, b, c, tmp.begin(), byY);
		copy_n(tmp.begin(), r - l, a);
		int k = 0;
		rep(i,l,r) {
			double dx = v[i].real() - x;
			if (dx * dx < best) tmp[k++] = v[i];
		}
		rep(i,0,k) rep(j,i+1,k) {
			double dy = tmp[j].imag() - tmp[i].imag();
			if (dy * dy >= best) break;
			upd(tmp[i], tmp[j]);
		}
	};
	rec(rec, 0, sz(v));
	return ret;
}
