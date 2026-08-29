#include "../../content/number-theory/FloorBlocks.h"

int main() {
	cin.tie(0)->sync_with_stdio(0);
	ll n;
	cin >> n;
	vector<ll> qs;
	floorBlocks(n, [&](ll, ll, ll q) { qs.pb(q); });
	reverse(all(qs));
	cout << sz(qs) << '\n';
	rep(i, 0, sz(qs)) cout << qs[i] << " \n"[i + 1 == sz(qs)];
}
