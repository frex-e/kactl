#include "../utilities/template.h"
#include "../../content/combinatorial/multinomial.h"

void check(vi v, ll want) {
	assert(multinomial(v) == want);
}

int main() {
	check({}, 1);
	check({0, 0, 0}, 1);
	check({30, 30}, 118264581564861424LL);
	check({0, 30, 0, 30}, 118264581564861424LL);
	// Pascal's triangle is independent of multiply/divide.
	// Up to n=60, even n times the largest coefficient fits.
	ll choose[61][61] = {};
	rep(n,0,61) {
		choose[n][0] = choose[n][n] = 1;
		rep(k,1,n)
			choose[n][k] = choose[n-1][k-1] + choose[n-1][k];
		rep(k,0,n+1) check({k, n-k}, choose[n][k]);
	}
	// Up to 20!, all intermediate products fit in ll.
	ll fact[21];
	fact[0] = 1;
	rep(i,1,21) fact[i] = fact[i-1] * i;
	mt19937 rng(20261008);
	rep(it,0,20000) {
		int total = (int)(rng()%21), left = total;
		vi v;
		rep(i,0,7) {
			int x = (int)(rng() % unsigned(left+1));
			v.push_back(x); left -= x;
		}
		v.push_back(left);
		shuffle(all(v), rng);
		ll want = fact[total];
		for (int x : v) want /= fact[x];
		check(v, want);
	}
	cout << "Tests passed!" << endl;
}
