#include "../utilities/template.h"
#include "../../content/geometry/kdTree.h"

void destroy(Node* n) {
	if (!n) return;
	destroy(n->first); destroy(n->second); delete n;
}
int main() {
	mt19937 rng(1729);
	auto rnd = [&]() { return (ll)(rng()%2001)-1000; };
	rep(it,0,2000) {
		vector<P> pts;
		rep(i,0,1+int(rng()%50)) pts.emplace_back(rnd(),rnd());
		KDTree tree(pts);
		rep(q,0,50) {
			P p(rnd(),rnd());
			ll best = LLONG_MAX;
			for (P x : pts) best = min(best,norm(x-p));
			auto [d,x] = tree.nearest(p);
			assert(d == best && norm(x-p) == best);
			assert(find(all(pts),x) != pts.end());
		}
		for (P p : pts) assert(tree.nearest(p).first == 0);
		destroy(tree.root);
	}
	cout << "Tests passed!" << endl;
}
