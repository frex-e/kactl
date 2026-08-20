#include "../utilities/template.h"

#include "../../content/data-structures/StaticRangeQuery.h"

void testSum(int n) {
	vi v(n);
	rep(i,0,n) v[i] = rand() % 21 - 10;
	RangeQuery rq(v);
	vi pref(n + 1);
	rep(i,0,n) pref[i + 1] = pref[i] + v[i];
	rep(l,0,n) rep(r,l+1,n+1)
		assert(rq.query(l, r) == pref[r] - pref[l]);
}

int main() {
	rep(n,1,120) testSum(n);
	rep(it,0,20) testSum(200 + rand() % 50);
	RangeQuery one({42});
	assert(one.query(0, 1) == 42);
	RangeQuery two({3, 4});
	assert(two.query(0, 1) == 3);
	assert(two.query(1, 2) == 4);
	assert(two.query(0, 2) == 7);
	cout << "Tests passed!" << endl;
}
