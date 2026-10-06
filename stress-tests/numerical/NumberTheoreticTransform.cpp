#include "../utilities/template.h"

#include "../../content/numerical/NumberTheoreticTransform.h"

vl simpleConv(vl a, vl b) {
	int s = sz(a) + sz(b) - 1;
	if (a.empty() || b.empty()) return {};
	vl c(s);
	rep(i,0,sz(a)) rep(j,0,sz(b))
		c[i+j] = (c[i+j] + (ll)a[i] * b[j]) % mod;
	for(auto &x: c) if (x < 0) x += mod;
	return c;
}

int ra() {
	static unsigned X;
	X *= 123671231;
	X += 1238713;
	X ^= 1237618;
	return (X >> 1);
}

void testSizes() {
	assert(conv({}, {1}).empty());
	assert(conv({1}, {}).empty());
	rep(p,0,10) rep(d,-1,2) rep(it,0,3) {
		int s = (1 << p) + d;
		if (s < 1) continue;
		int as = 1 + ra() % s;
		vl a(as), b(s - as + 1);
		for (auto &x : a) x = ra() % mod;
		for (auto &x : b) x = ra() % mod;
		assert(conv(a, b) == simpleConv(a, b));
	}
}

void testMaxSize() {
	// Dense inputs with an exact power-of-two result length.
	// (-1) * (-1) gives a triangular/trapezoidal coefficient count.
	int n = 1 << 23, as = n / 2, bs = n / 2 + 1;
	vl a(as, mod - 1), b(bs, mod - 1);
	vl c = conv(a, b);
	assert(sz(c) == n);
	rep(i,0,n) {
		ll expected = min({i + 1, as, bs, n - i});
		if (c[i] != expected) {
			cerr << "Maximum-size convolution mismatch at " << i
			     << ": got " << c[i] << ", expected " << expected
			     << endl;
			abort();
		}
	}
}

int main() {
	ll res = 0, res2 = 0;
	int ind = 0, ind2 = 0;
	vl a, b;
	rep(it,0,6000) {
		a.resize(ra() % 10);
		b.resize(ra() % 10);
		for(auto &x: a) x = (ra() % 100 - 50+mod)%mod;
		for(auto &x: b) x = (ra() % 100 - 50+mod)%mod;
		for(auto &x: simpleConv(a, b)) res += (ll)x * ind++ % mod;
		for(auto &x: conv(a, b)) res2 += (ll)x * ind2++ % mod;
		a.resize(16);
			vl a2 = a;
			ntt(a2);
			rep(k, 0, sz(a2)) {
				ll sum = 0;
				rep(x, 0, sz(a2)) { sum = (sum + a[x] * modpow<mod>(root, k * x * (mod - 1) / sz(a))) % mod; }
				assert(sum == a2[k]);
			}
	}
	assert(res==res2);
	testSizes();
	testMaxSize();
	testSizes(); // Reuse the cached roots at smaller sizes.
	cout<<"Tests passed!"<<endl;
}
