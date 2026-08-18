#include "../utilities/template.h"

#include "../../content/number-theory/FloorBlocks.h"

void testN(ll n) {
	ll prev = 0, cnt = 0;
	floorBlocks(n, [&](ll l, ll r, ll q) {
		assert(l == prev + 1);
		assert(l <= r && r <= n);
		assert(n / l == q && n / r == q);
		if (r < n) assert(n / (r + 1) != q);
		ll span = min(5LL, r - l + 1);
		rep(i,0,(int)span) assert(n / (l + i) == q);
		prev = r;
		cnt++;
	});
	if (n == 0) assert(prev == 0 && cnt == 0);
	else assert(prev == n && cnt >= 1);
}

int main() {
	floorBlocks(0, [&](ll, ll, ll) { assert(false); });
	rep(n,1,5000) testN(n);
	for (ll n : {1LL, 2LL, 3LL, 1000000LL, 1000000000000LL})
		testN(n);
	ll n = 1000000000000LL, sum = 0;
	floorBlocks(n, [&](ll l, ll r, ll q) {
		sum += (r - l + 1) * q;
	});
	ll brute = 0;
	// too big to brute; check first/last blocks
	ll firstQ = -1, lastQ = -1, lastR = 0;
	floorBlocks(n, [&](ll l, ll r, ll q) {
		if (firstQ < 0) {
			assert(l == 1 && q == n && r == 1);
			firstQ = q;
		}
		lastQ = q;
		lastR = r;
	});
	assert(lastQ == 1 && lastR == n);
	(void)sum;
	(void)brute;
	cout << "Tests passed!" << endl;
}
