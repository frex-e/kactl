#include "../utilities/template.h"
#include "../../content/data-structures/MonotonicCHT.h"

using T = __int128;
using Lines = vector<pair<ll, ll>>;

ll brute(const Lines& lines, ll x, bool minimum = false) {
	T best = minimum ? LLONG_MAX : LLONG_MIN;
	for (auto [k, m] : lines) {
		T v = T(k) * x + m;
		best = minimum ? min(best, v) : max(best, v);
	}
	assert(LLONG_MIN <= best && best <= LLONG_MAX);
	return (ll)best;
}

void check(const Lines& lines) {
	MonotonicCHT batch, online;
	Lines seen;
	rep(i,0,sz(lines)) {
		auto [k, m] = lines[i];
		batch.add(k, m);
		online.add(k, m);
		seen.push_back({k, m});
		ll x = i - 5;
		assert(online.query(x) == brute(seen, x));
		assert(online.query(x) == brute(seen, x));
	}
	rep(x,-5,6) assert(batch.query(x) == brute(lines, x));
}

void exhaustive(Lines& lines, int left) {
	if (!lines.empty()) check(lines);
	if (!left) return;
	ll lo = lines.empty() ? -2 : lines.back().first;
	for (ll k = lo; k <= 2; k++) {
		for (ll m = -2; m <= 2; m++) {
			lines.push_back({k, m});
			exhaustive(lines, left - 1);
			lines.pop_back();
		}
	}
}

void randomTests(bool minimum) {
	mt19937 rng(123);
	rep(it,0,1000) {
		MonotonicCHT h;
		Lines lines;
		ll k = minimum ? 1000 : -1000, x = -1000;
		rep(op,0,200) {
			if (lines.empty() || rng() % 3) {
				ll step = rng() % 4; // Include equal slopes.
				k += minimum ? -step : step;
				ll m = ll(rng() % 2000001) - 1000000;
				h.add(minimum ? -k : k, minimum ? -m : m);
				lines.push_back({k, m});
			} else {
				x += rng() % 20; // Include repeated queries.
				ll got = h.query(x);
				assert((minimum ? -got : got)
					== brute(lines, x, minimum));
			}
		}
		ll got = h.query(x);
		assert((minimum ? -got : got)
			== brute(lines, x, minimum));
	}
}

void regressions() {
	MonotonicCHT h;
	h.add(0, 0);
	h.add(1, -1);
	h.add(2, -4);
	assert(h.query(10) == 16); // Pointer at the tail.
	h.add(2, -5); // Worse parallel line is ignored.
	h.add(2, -4); // Identical line is ignored.
	assert(h.query(10) == 16);
	h.add(2, 1); // Replace tail and remove the middle.
	assert(h.query(10) == 21);
	h.add(3, 100); // Remove everything after the first line.
	assert(h.query(10) == 130);

	// Exact ties at an integer intersection, then advance.
	MonotonicCHT ties;
	rep(k,-10,11) ties.add(k, -7 * k);
	assert(ties.query(6) == 10);
	assert(ties.query(7) == 0);
	assert(ties.query(7) == 0);
	assert(ties.query(8) == 10);

	// Differences exceed ll and products exceed 64 bits.
	const ll v = 4000000000000000000LL;
	Lines lines{{LLONG_MIN, -v}, {0, v}, {LLONG_MAX, -v}};
	MonotonicCHT wide;
	for (auto [k, m] : lines) wide.add(k, m);
	rep(x,-1,2) assert(wide.query(x) == brute(lines, x));

	// Product overflows ll, while the final answer fits.
	MonotonicCHT cancellation;
	cancellation.add(LLONG_MAX, LLONG_MIN);
	assert(cancellation.query(2) == LLONG_MAX - 1);

	MonotonicCHT constant;
	constant.add(0, LLONG_MIN);
	assert(constant.query(LLONG_MIN) == LLONG_MIN);
	constant.add(0, LLONG_MAX);
	assert(constant.query(LLONG_MAX) == LLONG_MAX);
}

void largeHull() {
	const int n = 200000;
	MonotonicCHT h;
	rep(k,0,n) h.add(k, -ll(k) * k);
	rep(k,0,n) assert(h.query(2LL * k) == ll(k) * k);
	// Clamp a fully advanced pointer after popping the hull.
	h.add(n, 0);
	assert(h.query(2LL * n) == 2LL * n * n);
	h.add(n, 1);
	assert(h.query(2LL * n) == 2LL * n * n + 1);
}

int main() {
	Lines lines;
	exhaustive(lines, 4);
	randomTests(false);
	randomTests(true);
	regressions();
	largeHull();
	cout << "Tests passed!" << endl;
}
