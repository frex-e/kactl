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
	bool same(int a, int b) { return find(a) == find(b); }
};

void test(int n, int T, int k) {
	struct E { int l, r, a, b; };
	vector<E> es;
	rep(i,0,k) {
		int l = rand() % T, r = rand() % T + 1;
		if (l > r) swap(l, r);
		if (l == r) r = min(T, r + 1);
		es.push_back({l, r, rand()%n, rand()%n});
	}
	DynCon dc(n, T);
	for (auto e : es) dc.add(e.l, e.r, e.a, e.b);
	vector<vi> got(T, vi(n));
	dc.run([&](int t, RollbackUF& uf) {
		rep(i,0,n) got[t][i] = uf.find(i);
	});
	rep(t,0,T) {
		DSU d(n);
		for (auto e : es) if (e.l <= t && t < e.r)
			d.join(e.a, e.b);
		rep(i,0,n) rep(j,0,n)
			assert(d.same(i, j) ==
				(got[t][i] == got[t][j]));
	}
}

int main() {
	rep(n,1,12) rep(T,1,15) rep(it,0,30)
		test(n, T, rand() % 20);
	test(20, 40, 80);
	cout << "Tests passed!" << endl;
}
