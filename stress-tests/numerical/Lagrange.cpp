#include "../utilities/template.h"
#include "../../content/numerical/Lagrange.h"

template<ll mod>
ll eval(const vector<ll>& a, ll x) {
	x %= mod;
	if (x < 0) x += mod;
	ll ans = 0;
	for (int i = sz(a)-1; i >= 0; --i)
		ans = (ans*x + a[i]) % mod;
	return ans;
}

template<ll mod>
void check(const vector<ll>& a, const vector<ll>& xs) {
	vector<ll> y(sz(a));
	rep(i,0,sz(a)) y[i] = eval<mod>(a, i);
	for (ll x : xs)
		assert(lagrange<mod>(y, x) == eval<mod>(a, x));
}

template<ll mod>
void exhaustive() {
	vector<ll> xs;
	for (ll x = -mod; x <= 2*mod; ++x) xs.push_back(x);
	rep(n,1,mod+1) {
		vector<ll> a(n);
		ll count = 1;
		rep(i,0,n) count *= mod;
		for (ll mask = 0; mask < count; ++mask) {
			ll v = mask;
			for (ll& c : a) c = v % mod, v /= mod;
			check<mod>(a, xs);
		}
	}
}

template<ll mod>
void randomized(mt19937_64& rng) {
	rep(it,0,1000) {
		int n = 1 + (int)(rng() % min(200LL, mod));
		vector<ll> a(n);
		for (ll& c : a) c = (ll)(rng() % mod);
		if (it % 3 == 0) a.back() = 0;
		vector<ll> xs{0, n-1, n, mod-1, mod, mod+1,
			-mod, -mod-1, LLONG_MIN, LLONG_MAX};
		rep(i,0,10) {
			ll x = (ll)(rng() & (uint64_t)LLONG_MAX);
			xs.push_back(x);
			xs.push_back(-x);
		}
		check<mod>(a, xs);
	}
}

int main() {
	assert(lagrange({0, 1, 4}, 10) == 100);
	exhaustive<2>();
	exhaustive<3>();
	exhaustive<5>();
	mt19937_64 rng(42);
	randomized<7>(rng);
	randomized<101>(rng);
	randomized<998244353>(rng);
	randomized<1000000007>(rng);
	randomized<2147483647>(rng);

	// Independent prefix-sum oracle for the main application.
	rep(k,0,21) {
		vector<ll> sums(1001);
		rep(i,1,sz(sums)) {
			ll power = 1;
			rep(j,0,k) power = power*i % 1000000007;
			sums[i] = (sums[i-1]+power) % 1000000007;
		}
		vector<ll> y(sums.begin(), sums.begin()+k+2);
		rep(x,0,sz(sums)) assert(lagrange(y, x) == sums[x]);
	}
	cout << "Tests passed!" << endl;
}
