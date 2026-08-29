#include "../../content/number-theory/LinearSieve.h"

int main() {
	cin.tie(0)->sync_with_stdio(0);
	int n, a, b;
	cin >> n >> a >> b;
	linearSieve(n);
	int x = 0;
	for (int i = b; i < sz(pr); i += a) ++x;
	cout << sz(pr) << ' ' << x << '\n';
	bool first = true;
	for (int i = b; i < sz(pr); i += a) {
		if (!first) cout << ' ';
		first = false;
		cout << pr[i];
	}
	cout << '\n';
}
