#include "../utilities/template.h"
#include "../../content/data-structures/PrefixSum.h"
#include "../../content/data-structures/SubMatrix.h"

template<class T>
void check(const PrefixSum<T>& p, const vector<T>& a,
		const vi& l, const vi& r) {
	T want{};
	for (size_t i = 0; i < a.size(); ++i) {
		size_t index = i;
		bool inside = true;
		for (int d = sz(p.n); d--;) {
			int x = (int)(index % (size_t)p.n[d]);
			index /= (size_t)p.n[d];
			inside &= l[d] <= x && x < r[d];
		}
		if (inside) want += a[i];
	}
	assert(p.sum(l, r) == want);
}

void exhaustive(const vi& widths) {
	int count = 1;
	for (int w : widths) count *= w;
	vector<ll> a(count);
	for (ll& x : a) x = rand() % 21 - 10;
	PrefixSum<ll> p(widths, a);
	vi l(sz(widths)), r(sz(widths));
	auto boxes = [&](auto&& self, int d) -> void {
		if (d == sz(widths)) return check(p, a, l, r);
		rep(lo,0,widths[d]+1) rep(hi,lo,widths[d]+1) {
			l[d] = lo;
			r[d] = hi;
			self(self, d+1);
		}
	};
	boxes(boxes, 0);
}

int main() {
	exhaustive({4});
	exhaustive({2, 3});
	exhaustive({2, 3, 2});
	exhaustive({1, 2, 3, 1});
	exhaustive({2, 0, 3});
	exhaustive({0});

	rep(d,1,8) rep(it,0,80) {
		vi widths(d);
		int count = 1;
		for (int& w : widths) {
			w = rand() % 4 + 1;
			if (it % 10 == 0) w = rand() % 4;
			count *= w;
		}
		vector<ll> a(count);
		for (ll& x : a) x = (rand() % 21 - 10) * 1000000000LL;
		PrefixSum<ll> p(widths, a);
		check(p, a, vi(d), widths);
		rep(q,0,80) {
			vi l(d), r(d);
			rep(k,0,d) {
				l[k] = rand() % (widths[k] + 1);
				r[k] = rand() % (widths[k] + 1);
				if (l[k] > r[k]) swap(l[k], r[k]);
			}
			check(p, a, l, r);
		}
		// Every cell is a unit box, including edge cells.
		rep(i,0,count) {
			vi l(d), r(d);
			int index = i;
			for (int k = d; k--;) {
				l[k] = index % widths[k];
				r[k] = l[k] + 1;
				index /= widths[k];
			}
			assert(p.sum(l, r) == a[i]);
		}
	}

	// Dimension count is runtime and not limited by a bitmask.
	vi widths(70, 1), l(70);
	widths[3] = widths[68] = 2;
	vector<ll> a = {5, -7, 11, 13};
	PrefixSum<ll> many(widths, a);
	check(many, a, l, widths);
	l[3] = l[68] = 1;
	check(many, a, l, widths);
	l[69] = 1;
	check(many, a, l, widths);

	vector<double> fractions = {0.25, -0.5, 1.5, 2.75};
	PrefixSum<double> real({2, 2}, fractions);
	check(real, fractions, {0, 0}, {2, 2});
	check(real, fractions, {1, 0}, {2, 1});

	vector<vector<ll>> matrix = {{1, -2, 3}, {4, 5, -6}};
	SubMatrix<ll> old(matrix);
	PrefixSum<ll> flat({2, 3}, {1, -2, 3, 4, 5, -6});
	rep(u,0,3) rep(d,u,3) rep(lc,0,4) rep(rc,lc,4)
		assert(flat.sum({u, lc}, {d, rc})
			== old.sum(u, lc, d, rc));

	PrefixSum<ll> big({100, 10, 100}, vector<ll>(100000, 1));
	assert(big.sum({0, 0, 0}, {100, 10, 100}) == 100000);
	assert(big.sum({1, 2, 3}, {97, 8, 91}) == 96*6*88);
	cout << "Tests passed!" << endl;
}
