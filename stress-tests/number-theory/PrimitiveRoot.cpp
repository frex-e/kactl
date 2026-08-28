#include "../utilities/template.h"

#include "../../content/number-theory/PrimitiveRoot.h"

ull bruteOrd(ull a, ull p) {
	ull x = 1;
	rep(k,1,(int)p) {
		x = x * a % p;
		if (x == 1) return k;
	}
	return p - 1;
}

int main() {
	assert(ord(1, 2) == 1);
	assert(primitiveRoot(2) == 1);
	assert(primitiveRoot(3) == 2);
	assert(primitiveRoot(5) == 2);
	assert(primitiveRoot(7) == 3);
	assert(primitiveRoot(11) == 2);
	assert(primitiveRoot(13) == 2);
	assert(primitiveRoot(41) == 6);
	assert(primitiveRoot(998244353) == 3);
	assert(primitiveRoot(1000000007) == 5);

	rep(p,2,400) {
		if (!isPrime(p)) continue;
		ull g = primitiveRoot(p);
		assert(g >= 1 && g < (ull)p);
		assert(ord(g, p) == (ull)p - 1);
		set<ull> seen;
		ull x = 1;
		rep(i,0,p-1) {
			seen.insert(x);
			x = x * g % p;
		}
		assert(sz(seen) == p - 1);
		rep(a,1,p) {
			ull o = ord(a, p);
			assert(o == bruteOrd(a, p));
			assert(((ull)p - 1) % o == 0);
			assert(modpow(a, o, p) == 1);
		}
	}

	mt19937_64 rng(0);
	rep(it,0,30) {
		ull p;
		do { p = rng() % (1ull << 40) | 1; } while (!isPrime(p));
		ull g = primitiveRoot(p);
		assert(g >= 1 && g < p);
		assert(modpow(g, p - 1, p) == 1);
		for (ull q : set<ull>(all(factor(p - 1))))
			assert(modpow(g, (p - 1) / q, p) != 1);
		assert(ord(g, p) == p - 1);
		rep(i,0,8) {
			ull a = rng() % (p - 1) + 1;
			ull o = ord(a, p);
			assert(o >= 1 && o <= p - 1);
			assert((p - 1) % o == 0);
			assert(modpow(a, o, p) == 1);
			for (ull q : set<ull>(all(factor(o))))
				assert(modpow(a, o / q, p) != 1);
		}
	}
	cout << "Tests passed!" << endl;
}
