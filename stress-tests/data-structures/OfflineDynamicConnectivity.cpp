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

void test(int n, int ops) {
	DynCon dc(n, ops + 1);
	set<pii> live;
	vi expect;
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
	vi got = dc.ans();
	assert(got == expect);
}

int main() {
	rep(n,1,12) rep(it,0,80) test(n, rand() % 25 + 1);
	test(20, 80);
	DynCon dc(3, 8);
	dc.toggle(0, 1);
	dc.query();
	dc.toggle(1, 2);
	dc.query();
	dc.toggle(0, 1);
	dc.query();
	vi got = dc.ans();
	assert((got == vi{2, 1, 2}));
	cout << "Tests passed!" << endl;
}
