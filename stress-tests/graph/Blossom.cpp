#include "../utilities/template.h"

#include "../../content/graph/Blossom.h"
#include "../../content/graph/HopcroftKarp.h"
#include "../../content/graph/GeneralMatching.h"

void checkMatching(int n, const vector<pii>& ed, Blossom& b, int ans) {
	assert(sz(b.mate) == n);
	int seen = 0;
	rep(i,0,n) {
		int j = b.mate[i];
		if (j < 0) continue;
		assert(0 <= j && j < n);
		assert(b.mate[j] == i);
		if (i < j) seen++;
	}
	assert(seen == ans);
	set<pii> es;
	for (auto [u, v] : ed) {
		es.insert({u, v});
		es.insert({v, u});
	}
	rep(i,0,n) if (b.mate[i] > i)
		assert(es.count({i, b.mate[i]}));
}

int brute(int n, const vector<pii>& ed) {
	int m = sz(ed), best = 0;
	rep(mask,0,1 << m) {
		int used = 0, cnt = 0;
		bool ok = true;
		rep(i,0,m) if (mask >> i & 1) {
			int a = ed[i].first, b = ed[i].second;
			if (a == b || (used >> a & 1) || (used >> b & 1))
				ok = false;
			used |= 1 << a | 1 << b;
			cnt++;
		}
		if (ok) best = max(best, cnt);
	}
	return best;
}

void testBrute() {
	rep(n,0,7) rep(it,0,80) {
		vector<pii> ed;
		rep(i,0,n) rep(j,i+1,n) if (rand() % 2)
			ed.push_back({i, j});
		Blossom b(n);
		for (auto [u, v] : ed) b.ae(u, v);
		int ans = b.solve();
		assert(ans == brute(n, ed));
		checkMatching(n, ed, b, ans);
	}
	rep(it,0,150) {
		int n = rand() % 10 + 1;
		set<pii> s;
		int m = rand() % 13;
		rep(i,0,m) {
			int a = rand() % n, b = rand() % n;
			if (a > b) swap(a, b);
			if (a != b) s.insert({a, b});
		}
		vector<pii> ed(all(s));
		Blossom b(n);
		for (auto [u, v] : ed) b.ae(u, v);
		int ans = b.solve();
		assert(ans == brute(n, ed));
		checkMatching(n, ed, b, ans);
	}
}

void testBipartite() {
	rep(it,0,200) {
		int L = rand() % 12, R = rand() % 12;
		vector<vi> g(L);
		vector<pii> ed;
		rep(i,0,L) rep(j,0,R) if (rand() % 3 == 0) {
			g[i].push_back(j);
			ed.push_back({i, L + j});
		}
		vi match(R, -1);
		int hk = hopcroftKarp(g, match);
		Blossom b(L + R);
		for (auto [u, v] : ed) b.ae(u, v);
		int ans = b.solve();
		assert(ans == hk);
		checkMatching(L + R, ed, b, ans);
	}
}

void testVsAlgebraic() {
	rep(it,0,80) {
		int n = rand() % 12 + 1;
		vector<pii> ed;
		rep(i,0,n) rep(j,i+1,n) if (rand() % 3 == 0)
			ed.push_back({i, j});
		Blossom b(n);
		for (auto [u, v] : ed) b.ae(u, v);
		int ans = b.solve();
		checkMatching(n, ed, b, ans);
		vector<pii> gm = ed;
		int alg = sz(generalMatching(n, gm));
		assert(ans == alg);
	}
}

void testTriangleAndK4() {
	Blossom t(3);
	t.ae(0, 1); t.ae(1, 2); t.ae(2, 0);
	assert(t.solve() == 1);
	Blossom k(4);
	rep(i,0,4) rep(j,i+1,4) k.ae(i, j);
	assert(k.solve() == 2);
	Blossom e(5);
	assert(e.solve() == 0);
	Blossom s(2);
	s.ae(0, 1); s.ae(0, 1);
	assert(s.solve() == 1);
	assert(s.mate[0] == 1 && s.mate[1] == 0);
}

int main() {
	testTriangleAndK4();
	testBrute();
	testBipartite();
	testVsAlgebraic();
	cout << "Tests passed!" << endl;
}
