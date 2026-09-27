#include "../utilities/template.h"
#include "../../content/data-structures/CartesianTree.h"

void check(const vi& a) {
	vi expected(sz(a), -1);
	vi left = expected, right = expected;
	auto build = [&](auto&& self, int l, int r, int p) -> void {
		if (l == r) return;
		int m = int(min_element(a.begin() + l, a.begin() + r)
			- a.begin());
		expected[m] = p;
		if (p != -1) (m < p ? left[p] : right[p]) = m;
		self(self, l, m, m);
		self(self, m + 1, r, m);
	};
	build(build, 0, sz(a), -1);
	auto [p, l, r] = cartesianTree(a);
	assert(p == expected && l == left && r == right);
}

int main() {
	// Exhaust all short arrays over {-1, 0, 1}.
	int count = 1;
	rep(n,0,11) {
		rep(mask,0,count) {
			vi a(n);
			int x = mask;
			for (int& v : a) v = x % 3 - 1, x /= 3;
			check(a);
		}
		count *= 3;
	}

	mt19937 rng(123);
	rep(it,0,10000) {
		vi a(rng() % 100);
		for (int& v : a) v = int(rng() % 11) - 5;
		check(a);
	}
	check({INT_MAX, INT_MIN, INT_MIN, INT_MAX});

	// Long chains and a final pop of the whole stack.
	int n = 200000;
	vi a(n, 7), chain(n);
	iota(all(chain), -1);
	assert(cartesianTree(a)[0] == chain);
	iota(all(a), 0);
	assert(cartesianTree(a)[0] == chain);
	reverse(all(a));
	iota(all(chain), 1);
	chain.back() = -1;
	assert(cartesianTree(a)[0] == chain);
	iota(all(a), 0);
	a.back() = -1;
	iota(all(chain), -1);
	chain[0] = n - 1;
	chain.back() = -1;
	assert(cartesianTree(a)[0] == chain);
	cout << "Tests passed!" << endl;
}
