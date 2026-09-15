#include "../utilities/template.h"

#include "../../content/graph/LinkCutTree.h"
#include "../../content/data-structures/UnionFind.h"

int main() {
	{
		LinkCut lc(5);
		rep(i,0,5) assert(lc.root(i) == i);
		lc.link(0, 1); // keep 1
		assert(lc.root(0) == 1 && lc.root(1) == 1);
		lc.link(2, 3); // keep 3
		assert(lc.root(2) == 3 && lc.root(3) == 3);
		lc.link(0, 2); // keep 2's tree (root 3)
		rep(i,0,4) assert(lc.root(i) == 3);
		assert(lc.root(4) == 4);
		lc.link(4, 1); // keep 1's tree (root 3)
		rep(i,0,5) assert(lc.root(i) == 3);
		lc.cut(0, 2);
		assert(lc.connected(0, 1) && !lc.connected(0, 2));
		assert(lc.root(0) == lc.root(1) && lc.root(1) == lc.root(4));
		assert(lc.root(2) == lc.root(3));
	}
	{
		LinkCut lc(4);
		lc.link(0, 1);
		lc.link(2, 3);
		lc.link(1, 3); // keep 3
		rep(i,0,4) assert(lc.root(i) == 3);
	}
	{
		LinkCut lc(4);
		lc.link(0, 1);
		lc.link(2, 3);
		lc.link(3, 1); // keep 1
		rep(i,0,4) assert(lc.root(i) == 1);
	}

	srand(2);
	LinkCut lczero(0);
	rep(it,0,10000) {
		int N = rand() % 20 + 1;
		LinkCut lc(N);
		UF uf(N);
		vector<pii> edges;
		rep(i,0,N) assert(lc.root(i) == i);
		auto check = [&]() {
			uf.e.assign(N, -1);
			for (auto &ed: edges) uf.join(ed.first, ed.second);
			vi seen(N, -1);
			rep(i,0,N) {
				int r = lc.root(i);
				assert(0 <= r && r < N);
				assert(uf.sameSet(i, r));
				assert(lc.root(r) == r);
				int f = uf.find(i);
				if (seen[f] < 0) seen[f] = r;
				else assert(seen[f] == r);
			}
		};
		rep(it2,0,1000) {
			int v = (rand() >> 4) & 3;
			if (v == 0 && !edges.empty()) { // remove
				int r = (rand() >> 4) % sz(edges);
				pii ed = edges[r];
				swap(edges[r], edges.back());
				edges.pop_back();
				if (rand() & 16)
					lc.cut(ed.first, ed.second);
				else
					lc.cut(ed.second, ed.first);
			} else {
				int a = (rand() >> 4) % N;
				int b = (rand() >> 4) % N;
				uf.e.assign(N, -1);
				for (auto &ed: edges) uf.join(ed.first, ed.second);
				bool c = uf.sameSet(a, b);
				if (!c && v != 1) {
					int keepA = rand() & 32;
					int old = keepA ? lc.root(a) : lc.root(b);
					if (keepA) lc.link(b, a);
					else lc.link(a, b);
					edges.emplace_back(a, b);
					assert(lc.root(a) == old && lc.root(b) == old);
					rep(i,0,N) if (uf.sameSet(i, a) || uf.sameSet(i, b))
						assert(lc.root(i) == old);
				} else {
					assert(lc.connected(a, b) == c);
				}
			}
			if (it2 % 50 == 0) check();
		}
		check();
	}
	cout<<"Tests passed!"<<endl;
}
