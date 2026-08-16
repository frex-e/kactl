#include "../utilities/template.h"

#include "../../content/data-structures/LazySegmentTree.h"

int main() {
	rep(n,1,30) {
		LazyUpdateTree tr(n);
		vi v(n);
		rep(it,0,20000) {
			int l = rand() % n, r = rand() % n;
			if (l > r) swap(l, r);
			int x = rand() % 11 - 5;
			if (rand() % 2) {
				tr.update(l, r, x);
				rep(i,l,r+1) v[i] += x;
			} else {
				int got = tr.query(l, r);
				int exp = INT_MIN;
				rep(i,l,r+1) exp = max(exp, v[i]);
				assert(got == exp);
			}
		}
	}
	cout << "Tests passed!" << endl;
}
