#include "../utilities/template.h"

#include "../../content/number-theory/ModSum.h"

ll rmod(ll x, ll m) {
	x %= m;
	return x < 0 ? x + m : x;
}

ll rdiv(ll x, ll y) {
	x -= rmod(x, y);
	return x / y;
}

ll modsum_naive(ll to, ll c, ll k, ll m) {
	ll res = 0;
	for (int i = 0; i < (int)to; ++i)
		res += rmod(c + k * i, m);
	return res;
}

ll divsum_naive(ll to, ll c, ll k, ll m) {
	ll res = 0;
	for (int i = 0; i < (int)to; ++i)
		res += rdiv(c + k * i, m);
	return res;
}

// Iterative floor-sum reference; keep intermediates exact instead
// of relying on the snippet's unsigned wraparound cancellation.
__uint128_t divsum_wide(ull to, ull c, ull k, ull m) {
	__uint128_t n = to, b = c, a = k, mod = m, res = 0;
	for (;;) {
		res += n * (n - 1) / 2 * (a / mod);
		res += n * (b / mod);
		a %= mod; b %= mod;
		__uint128_t y = a * n + b;
		if (y < mod) return res;
		n = y / mod; b = y % mod;
		swap(a, mod);
	}
}

void compare() {
	rep(to,0,30) {
		rep(c,-30,30) {
			rep(k,-30,30) {
				rep(m,1,30) {
					ll a = modsum(to, c, k, m);
					ll b = modsum_naive(to, c, k, m);
					if (a != b) {
						cout << "differ! " << to << ' ' << c << ' ' << k << ' ' << m << ": " << a << " vs " << b << endl;
						assert(false);
					}
				}
			}
		}
	}
}

void compare2() {
	rep(to,0,30) {
		rep(c,0,30) {
			rep(k,0,30) {
				rep(m,1,30) {
					ll a = divsum(to, c, k, m);
					ll b = divsum_naive(to, c, k, m);
					assert(divsum_wide(to, c, k, m)
						== (__uint128_t)b);
					if (a != b) {
						cout << "differ! " << to << ' ' << c << ' ' << k << ' ' << m << ": " << a << " vs " << b << endl;
						assert(false);
					}
				}
			}
		}
	}
}

int main() {
	compare(); compare2();
	assert(modsum((ll)1e18, 1, 2, 3) == (ll)1e18);
	// First macOS rand() case: the exact sum is correct, but the
	// old statistical comparison with to*m/2 failed its tolerance.
	assert(modsum_naive(134456, 1129900996, 6490600292,
		246235914) == 16554116784994);
	assert(modsum(134456, 1129900996, 6490600292,
		246235914) == 16554116784994);

	mt19937_64 rng(1);
	rep(i,0,10000) {
		ll t = (ll)(rng() % (1ULL << 34));
		ll c = (ll)(rng() % (1ULL << 34)) - (1LL << 33);
		ll k = (ll)(rng() % (1ULL << 34)) - (1LL << 33);
		ll m = 1 + (ll)(rng() % (1ULL << 29));
		ull b = (ull)rmod(c, m), a = (ull)rmod(k, m);
		__uint128_t expected = (__uint128_t)t * b
			+ (__uint128_t)a * t * (t - 1) / 2
			- (__uint128_t)m * divsum_wide(t, b, a, m);
		assert(expected <= (__uint128_t)LLONG_MAX);
		assert(modsum(t, c, k, m) == (ll)expected);
		// Bounds keep to*k+c within uint64_t. The floor sum itself
		// may overflow uint64_t, so compare modulo 2^64.
		b = rng() % (1ULL << 29);
		a = rng() % (1ULL << 29);
		assert(divsum(t, b, a, m)
			== (ull)divsum_wide(t, b, a, m));
	}
	cout<<"Tests passed!"<<endl;
	return 0;
}
