#include "../utilities/template.h"
#include "../../content/data-structures/WaveletTree.h"

void check(const WaveletTree& w, const vi& a, int l, int r,
		const vector<ll>& thresholds, bool exhaustive = false) {
	vi sorted(a.begin() + l, a.begin() + r);
	sort(all(sorted));
	rep(k,0,sz(sorted)) assert(w.kth(l, r, k) == sorted[k]);
	if (l < r)
		assert(w.kth(l, r, (r-l-1)/2) == sorted[(r-l-1)/2]);
	for (ll x : thresholds) {
		int less = (int)(lower_bound(all(sorted), x)
			- sorted.begin());
		int leq = (int)(upper_bound(all(sorted), x)
			- sorted.begin());
		assert(w.countLess(l, r, x) == less);
		// Strict predecessor and inclusive successor.
		if (less) assert(w.kth(l, r, less-1) == sorted[less-1]);
		if (less < r-l)
			assert(w.kth(l, r, less) == sorted[less]);
		// Inclusive predecessor and strict successor.
		if (x < LLONG_MAX) {
			int k = w.countLess(l, r, x+1);
			assert(k == leq);
			if (k) assert(w.kth(l, r, k-1) == sorted[leq-1]);
			if (k < r-l) assert(w.kth(l, r, k) == sorted[leq]);
		}
		assert(w.count(l, r, x, x) == 0);
	}
	vi distinct = sorted;
	distinct.erase(unique(all(distinct)), distinct.end());
	for (int x : distinct) {
		int freq = (int)count(all(sorted), x);
		assert(w.count(l, r, x, x+1LL) == freq);
	}
	rep(i,0,sz(thresholds)) {
		int end = exhaustive ? sz(thresholds) : i+2;
		rep(j,i,min(end, sz(thresholds))) {
			ll lo = thresholds[i], hi = thresholds[j];
			int want = 0;
			for (int x : sorted) want += lo <= x && x < hi;
			assert(w.count(l, r, lo, hi) == want);
		}
	}
	assert(w.count(l, r, LLONG_MIN, LLONG_MAX) == r-l);
}

void allRanges(const vi& a, const vector<ll>& thresholds) {
	vi copy = a;
	const WaveletTree w(copy);
	assert(copy == a);
	rep(l,0,sz(a)+1) rep(r,l,sz(a)+1)
		check(w, a, l, r, thresholds, true);
}

int main() {
	WaveletTree w({5, 1, 5, -2, 7});
	assert(w.kth(1, 5, 1) == 1);
	assert(w.count(0, 5, 5, 6) == 2);
	assert(w.kth(0, 5, (5-1)/2) == 5);
	int k = w.countLess(0, 5, 2);
	assert(k < 5 && w.kth(0, 5, k) == 5);

	// Every array of length <= 6 over {-2, 0, 2}.
	rep(n,0,7) {
		int ways = 1;
		rep(i,0,n) ways *= 3;
		rep(mask,0,ways) {
			vi a(n);
			int digits = mask;
			for (int& x : a) {
				x = 2 * (digits % 3) - 2;
				digits /= 3;
			}
			allRanges(a, {-3, -2, -1, 0, 1, 2, 3});
		}
	}

	vector<ll> bounds = {LLONG_MIN, (ll)INT_MIN-1,
		INT_MIN, (ll)INT_MIN+1, -1, 0, 1,
		(ll)INT_MAX-1, INT_MAX, (ll)INT_MAX+1, LLONG_MAX};
	allRanges({}, bounds);
	allRanges({INT_MIN}, bounds);
	allRanges({INT_MAX, INT_MAX, INT_MAX}, bounds);
	allRanges({INT_MIN, INT_MAX, 0, INT_MIN, -1,
		INT_MAX, 1, INT_MIN+1, INT_MAX-1}, bounds);

	mt19937 rng(12345);
	vi pool = {INT_MIN, INT_MIN+1, -1, 0, 1,
		INT_MAX-1, INT_MAX};
	rep(it,0,1000) {
		int n = (int)(rng() % 101);
		vi a(n);
		for (int& x : a) {
			if (it % 3 == 0) x = (int)(rng() % 11) - 5;
			else if (it % 3 == 1) x = pool[rng() % sz(pool)];
			else x = (int)(rng() % 2000000001) - 1000000000;
		}
		const WaveletTree tree(a);
		rep(q,0,50) {
			int l = (int)(rng() % (n+1));
			int r = (int)(rng() % (n+1));
			if (l > r) swap(l, r);
			ll x = (ll)(rng() % 4000000001ULL) - 2000000000;
			vector<ll> thresholds = bounds;
			thresholds.push_back(x);
			if (n) thresholds.push_back(a[rng() % n]);
			sort(all(thresholds));
			check(tree, a, l, r, thresholds);
		}
	}

	// Many distinct values, both index orders.
	int n = 200000;
	vi a(n);
	rep(i,0,n) a[i] = 2*i - n;
	rep(rev,0,2) {
		const WaveletTree tree(a);
		rep(q,0,1000) {
			int l = (int)(rng() % n);
			int r = l + 1 + (int)(rng() % (n-l));
			int rank = (int)(rng() % (r-l));
			int val = a[rev ? r-1-rank : l+rank];
			assert(tree.kth(l, r, rank) == val);
			assert(tree.countLess(l, r, val) == rank);
			assert(tree.count(l, r, val, val+1LL) == 1);
		}
		reverse(all(a));
	}

	// Highly uneven frequencies and long runs of duplicates.
	a.assign(n, 0);
	rep(i,0,257) a[3*i] = i-128;
	const WaveletTree skewed(a);
	check(skewed, a, 0, n, {-129, -1, 0, 1, 129});
	cout << "Tests passed!" << endl;
}
