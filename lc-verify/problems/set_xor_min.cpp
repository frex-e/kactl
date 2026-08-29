#include "../../content/data-structures/BinaryTrie.h"

int main() {
	cin.tie(0)->sync_with_stdio(0);
	int q;
	cin >> q;
	BinaryTrie t;
	rep(i, 0, q) {
		int ty, x;
		cin >> ty >> x;
		if (ty == 0) t.insert(x);
		else if (ty == 1) t.erase(x);
		else cout << t.minxor(x) << '\n';
	}
}
