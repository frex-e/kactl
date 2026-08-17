#include "../utilities/template.h"

#include "../../content/number-theory/Mobius.h"

int naiveMu(int n) {
	if (n <= 0) return 0;
	int cnt = 0;
	for (int p = 2; p * p <= n; p++) {
		if (n % p == 0) {
			n /= p;
			if (n % p == 0) return 0;
			cnt++;
		}
	}
	if (n > 1) cnt++;
	return cnt % 2 ? -1 : 1;
}

int main() {
	vi empty;
	calcMobius(empty);
	assert(empty.empty());

	vi one(1);
	calcMobius(one);
	assert(one[0] == 0);

	const int N = 20000;
	vi mu(N);
	calcMobius(mu);
	assert(mu[0] == 0);
	assert(mu[1] == 1);
	rep(i,0,N) assert(mu[i] == naiveMu(i));

	rep(n,2,200) {
		vi a(n);
		calcMobius(a);
		rep(i,0,n) assert(a[i] == naiveMu(i));
	}
	cout << "Tests passed!" << endl;
}
