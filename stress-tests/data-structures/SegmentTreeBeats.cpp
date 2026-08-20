#include "../utilities/template.h"

#define fr first
#define sc second

#include "../../content/data-structures/SegmentTreeBeats.h"

int main() {
	rep(n,1,25) {
		vector<ll> a(n);
		rep(i,0,n) a[i] = rand() % 21 - 10;
		SegmentTreeBeats tr(a);
		vector<ll> v = a;
		rep(it,0,10000) {
			int op = rand() % 6;
			int l = rand() % n, r = rand() % n;
			if (l > r) swap(l, r);
			if (op == 0) {
				ll x = rand() % 21 - 10;
				tr.chmin(l, r, x);
				rep(i,l,r+1) v[i] = min(v[i], x);
			} else if (op == 1) {
				ll x = rand() % 21 - 10;
				tr.chmax(l, r, x);
				rep(i,l,r+1) v[i] = max(v[i], x);
			} else if (op == 2) {
				ll x = rand() % 11 - 5;
				tr.add(l, r, x);
				rep(i,l,r+1) v[i] += x;
			} else if (op == 3) {
				ll got = tr.qsum(l, r), exp = 0;
				rep(i,l,r+1) exp += v[i];
				assert(got == exp);
			} else if (op == 4) {
				ll got = tr.qmin(l, r), exp = LLONG_MAX;
				rep(i,l,r+1) exp = min(exp, v[i]);
				assert(got == exp);
			} else {
				ll got = tr.qmax(l, r), exp = LLONG_MIN;
				rep(i,l,r+1) exp = max(exp, v[i]);
				assert(got == exp);
			}
		}
	}
	{
		SegmentTreeBeats tr(5);
		assert(tr.qsum(0, 4) == 0);
		tr.add(1, 3, 4);
		assert(tr.qsum(0, 4) == 12);
		assert(tr.qmin(0, 4) == 0);
		assert(tr.qmax(0, 4) == 4);
		tr.chmin(0, 4, 2);
		assert(tr.qsum(0, 4) == 6);
		assert(tr.qmax(0, 4) == 2);
		tr.chmax(0, 2, 5);
		assert(tr.qmin(0, 4) == 0);
		assert(tr.qmax(0, 4) == 5);
		assert(tr.qsum(0, 4) == 5+5+5+2+0);
	}
	cout << "Tests passed!" << endl;
}
