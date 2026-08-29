#include "../../content/strings/SuffixArray.h"

int main() {
	cin.tie(0)->sync_with_stdio(0);
	string s;
	cin >> s;
	int n = sz(s);
	SuffixArray sa(s);
	ll ans = 1LL * n * (n + 1) / 2;
	rep(i, 1, n + 1) ans -= sa.lcp[i];
	cout << ans << '\n';
}
