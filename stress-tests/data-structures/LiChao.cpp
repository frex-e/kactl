#include "../utilities/template.h"

#include "../../content/data-structures/LiChao.h"
#include "../../content/data-structures/LineContainer.h"

ll bruteMin(const vector<pair<ll,ll>>& ls, ll x) {
	ll best = LLONG_MAX / 4;
	for (auto [a, b] : ls) best = min(best, a * x + b);
	return best;
}

void testVsBrute() {
	rep(it,0,200) {
		int L = rand() % 40 - 20;
		int width = rand() % 40 + 1;
		int R = L + width;
		LiChao hull(L, R);
		vector<pair<ll,ll>> ls;
		assert(hull.query(L) == LiChao::INF);
		rep(j,0,80) {
			ll a = rand() % 41 - 20;
			ll b = rand() % 2001 - 1000;
			hull.insert({a, b});
			ls.push_back({a, b});
			int x = L + rand() % (R - L);
			assert(hull.query(x) == bruteMin(ls, x));
		}
		rep(x,L,R) assert(hull.query(x) == bruteMin(ls, x));
	}
}

void testVsLineContainer() {
	rep(it,0,100) {
		int L = 0, R = 50;
		LiChao hull(L, R);
		LineContainer mx;
		vector<pair<ll,ll>> ls;
		rep(j,0,60) {
			ll k = rand() % 41 - 20;
			ll m = rand() % 2001 - 1000;
			mx.add(k, m);
			hull.insert({-k, -m});
			ls.push_back({k, m});
			int x = rand() % (R - L);
			ll gotMax = -hull.query(x);
			assert(gotMax == mx.query(x));
		}
	}
}

void testParallelAndLeaf() {
	LiChao hull(0, 1);
	hull.insert({5, 3});
	assert(hull.query(0) == 3);
	hull.insert({5, 1});
	assert(hull.query(0) == 1);
	LiChao h2(-3, 5);
	h2.insert({0, 10});
	h2.insert({0, 4});
	rep(x,-3,5) assert(h2.query(x) == 4);
}

int main() {
	testVsBrute();
	testVsLineContainer();
	testParallelAndLeaf();
	cout << "Tests passed!" << endl;
}
