#include "../../content/strings/SuffixArray.h"

int main() {
	cin.tie(0)->sync_with_stdio(0);
	string s;
	cin >> s;
	SuffixArray sa(s);
	int n = sz(s);
	rep(i, 1, n + 1) cout << sa.sa[i] << " \n"[i == n];
}
