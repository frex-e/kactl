#include "../../content/data-structures/SegmentTreeBeats.h"

int main() {
	cin.tie(0)->sync_with_stdio(0);
	int n, q;
	cin >> n >> q;
	vector<ll> a(n);
	rep(i, 0, n) cin >> a[i];
	SegmentTreeBeats st(a);
	rep(i, 0, q) {
		int ty, l, r;
		cin >> ty >> l >> r;
		--r;
		if (ty == 0) {
			ll b;
			cin >> b;
			st.chmin(l, r, b);
		} else if (ty == 1) {
			ll b;
			cin >> b;
			st.chmax(l, r, b);
		} else if (ty == 2) {
			ll b;
			cin >> b;
			st.add(l, r, b);
		} else {
			cout << st.qsum(l, r) << '\n';
		}
	}
}
