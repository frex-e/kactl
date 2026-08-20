#include "../utilities/template.h"

#include "../../content/data-structures/MonotonicMap.h"

const ll INF = LLONG_MAX / 4;
const int HI = 1000;

template<class Prefer>
ll mergeVal(ll a, ll b) {
	return Prefer{}(a, b) ? a : b;
}

// Suffix: best among keys >= q. Prefix: best among keys <= q.
template<bool SUFFIX, class Prefer>
void testOnce(int n, int ops) {
	using Dir = conditional_t<SUFFIX, less<int>, greater<int>>;
	RangeQuery<Dir, Prefer> rq;
	if constexpr (SUFFIX)
		rq.ins(HI + 1, Prefer{}(0, 1) ? INF : -INF);
	else
		rq.ins(-1, Prefer{}(0, 1) ? INF : -INF);

	vector<pair<int, ll>> pts;
	rep(i,0,ops) {
		int k = rand() % (HI + 1);
		ll v = rand() % 2001 - 1000;
		rq.ins(k, v);
		pts.emplace_back(k, v);
		rep(q,0,HI+1) {
			ll want = Prefer{}(0, 1) ? INF : -INF;
			for (auto [pk, pv] : pts) {
				if (SUFFIX ? pk >= q : pk <= q)
					want = mergeVal<Prefer>(want, pv);
			}
			assert(rq.query(q) == want);
		}
	}
	(void)n;
}

int main() {
	rep(it,0,30) {
		int ops = 1 + rand() % 40;
		testOnce<true, less_equal<ll>>(0, ops);
		testOnce<true, greater_equal<ll>>(0, ops);
		testOnce<false, less_equal<ll>>(0, ops);
		testOnce<false, greater_equal<ll>>(0, ops);
	}
	RangeQuery<less<int>, less_equal<ll>> sufMin;
	sufMin.ins(100, INF);
	sufMin.ins(5, 3);
	sufMin.ins(10, 1);
	sufMin.ins(7, 4);
	assert(sufMin.query(0) == 1);
	assert(sufMin.query(6) == 1);
	assert(sufMin.query(8) == 1);
	assert(sufMin.query(10) == 1);
	assert(sufMin.query(11) == INF);

	RangeQuery<greater<int>, greater_equal<ll>> prefMax;
	prefMax.ins(-1, -INF);
	prefMax.ins(5, 3);
	prefMax.ins(10, 1);
	prefMax.ins(7, 4);
	assert(prefMax.query(100) == 4);
	assert(prefMax.query(7) == 4);
	assert(prefMax.query(6) == 3);
	assert(prefMax.query(4) == -INF);

	cout << "Tests passed!" << endl;
}
