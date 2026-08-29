#include "../../content/data-structures/LiChao.h"

int main() {
	cin.tie(0)->sync_with_stdio(0);
	int n, q;
	cin >> n >> q;
	const int L = -1'000'000'000, R = 1'000'000'001;
	LiChao cht(L, R);
	rep(i, 0, n) {
		ll a, b;
		cin >> a >> b;
		cht.insert(LiChao::Fn(a, b));
	}
	rep(i, 0, q) {
		int ty;
		cin >> ty;
		if (ty == 0) {
			ll a, b;
			cin >> a >> b;
			cht.insert(LiChao::Fn(a, b));
		} else {
			int p;
			cin >> p;
			cout << cht.query(p) << '\n';
		}
	}
}
