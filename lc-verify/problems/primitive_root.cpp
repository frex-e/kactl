#include "../../content/number-theory/PrimitiveRoot.h"

int main() {
	cin.tie(0)->sync_with_stdio(0);
	int q;
	cin >> q;
	rep(i, 0, q) {
		ull p;
		cin >> p;
		cout << primitiveRoot(p) << '\n';
	}
}
