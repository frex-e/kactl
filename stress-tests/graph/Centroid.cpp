#include "../utilities/template.h"
#include "../utilities/genTree.h"

#include "../../content/graph/Centroid.h"

void checkTree(int n, const vector<pii>& edges) {
	Centroid cd(n);
	for (auto [a, b] : edges) cd.ae(a, b);
	vi seen;
	cd.decomp(0, [&](int c) {
		int ncomp = cd.dfsSz(c, -1);
		int mx = 0;
		for (int v : cd.g[c]) if (!cd.banned[v])
			mx = max(mx, cd.sz[v]);
		assert(mx <= ncomp / 2);
		assert(ncomp - 1 - mx <= ncomp / 2);
		seen.push_back(c);
	});
	assert(sz(seen) == n);
	sort(all(seen));
	rep(i,0,n) assert(seen[i] == i);
}

int main() {
	Centroid one(1);
	int hits = 0;
	one.decomp(0, [&](int c) { assert(c == 0); hits++; });
	assert(hits == 1);

	rep(n,2,40) rep(it,0,50) {
		auto e = genRandomTree(n);
		checkTree(n, e);
	}
	cout << "Tests passed!" << endl;
}
