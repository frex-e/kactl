#include "../../content/number-theory/Factor.h"

int main() {
	cin.tie(0)->sync_with_stdio(0);
	int q;
	cin >> q;
	rep(i, 0, q) {
		ull a;
		cin >> a;
		auto f = factor(a);
		sort(all(f));
		cout << sz(f);
		for (ull x : f) cout << ' ' << x;
		cout << '\n';
	}
}
