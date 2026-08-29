#include "../utilities/template.h"

#include "../../content/data-structures/OfflineDynamicConnectivity.h"

struct DSU {
	vi e;
	DSU(int n) : e(n, -1) {}
	int find(int x) {
		return e[x] < 0 ? x : e[x] = find(e[x]);
	}
	void join(int a, int b) {
		a = find(a), b = find(b);
		if (a != b) e[a] = b;
	}
};

int comps(int n, const set<pii>& es) {
	DSU d(n);
	for (auto [a, b] : es) d.join(a, b);
	int c = 0;
	rep(i,0,n) if (d.find(i) == i) c++;
	return c;
}

ll compsum(int n, const set<pii>& es, const vector<ll>& a, int v) {
	DSU d(n);
	for (auto [x, y] : es) d.join(x, y);
	int r = d.find(v);
	ll s = 0;
	rep(i,0,n) if (d.find(i) == r) s += a[i];
	return s;
}

void test(int n, int ops) {
	DynCon dc(n, ops + 1);
	set<pii> live;
	vector<ll> expect;
	rep(i,0,ops) {
		int ty = rand() % 3;
		if (ty == 0 || live.empty()) {
			int a = rand() % n, b = rand() % n;
			pii e = {min(a, b), max(a, b)};
			dc.toggle(a, b);
			if (live.count(e)) live.erase(e);
			else live.insert(e);
		} else if (ty == 1) {
			auto it = live.begin();
			advance(it, rand() % sz(live));
			dc.toggle(it->first, it->second);
			live.erase(it);
		} else {
			dc.query();
			expect.push_back(comps(n, live));
		}
	}
	vector<ll> got = dc.ans();
	assert(got == expect);
}

void testSums(int n, int ops) {
	vector<ll> a(n);
	rep(i,0,n) a[i] = rand() % 10;
	DynCon dc(n, ops + 1, a);
	set<pii> live;
	vector<ll> expect;
	rep(i,0,ops) {
		int ty = rand() % 5;
		if (ty <= 1) {
			int x = rand() % n, y = rand() % n;
			if (x == y) continue;
			if (x > y) swap(x, y);
			dc.toggle(x, y);
			if (live.count({x, y})) live.erase({x, y});
			else live.insert({x, y});
		} else if (ty == 2) {
			int v = rand() % n, x = rand() % 7;
			dc.addVal(v, x);
			a[v] += x;
		} else if (ty == 3) {
			int v = rand() % n;
			dc.query(v);
			expect.push_back(compsum(n, live, a, v));
		} else {
			dc.query();
			expect.push_back(comps(n, live));
		}
	}
	vector<ll> got = dc.ans();
	assert(got == expect);
}

int main() {
	rep(n,1,12) rep(it,0,80) test(n, rand() % 25 + 1);
	test(20, 80);
	rep(n,1,10) rep(it,0,40) testSums(n, rand() % 20 + 1);
	DynCon dc(3, 8);
	dc.toggle(0, 1);
	dc.query();
	dc.toggle(1, 2);
	dc.query();
	dc.toggle(0, 1);
	dc.query();
	vector<ll> got = dc.ans();
	assert((got == vector<ll>{2, 1, 2}));
	DynCon ds(3, 16, vector<ll>{1, 2, 3});
	ds.query(0);
	ds.toggle(0, 1);
	ds.query(0);
	ds.addVal(1, 10);
	ds.query(0);
	ds.query(2);
	got = ds.ans();
	assert((got == vector<ll>{1, 3, 13, 3}));
	cout << "Tests passed!" << endl;
}
