#include "../utilities/template.h"
#include "../../content/various/BigInt.h"

using u128 = __uint128_t;
mt19937_64 rng(123456789);

string normalize(string s) {
	size_t i = s.find_first_not_of('0');
	return i == string::npos ? "0" : s.substr(i);
}

template<int N> string wrap(string s) {
	if (sz(s) > 9*N) s = s.substr(s.size() - 9*N);
	return normalize(s);
}

// Independent oracle: one decimal digit per position.
template<int N> string combine(string a, string b,
		bool subtract = false) {
	a = string(9*N-sz(a), '0') + a;
	b = string(9*N-sz(b), '0') + b;
	string s(9*N, '0');
	int carry = 0;
	for (int i = 9*N; i--;) {
		int t = a[i]-'0' + carry +
			(subtract ? -(b[i]-'0') : b[i]-'0');
		s[i] = char('0' + (t+10)%10);
		carry = subtract ? -(t < 0) : t / 10;
	}
	return normalize(s);
}

string multiply(string s, unsigned b) {
	ull carry = 0;
	for (int i = sz(s); i--;) {
		ull t = ull(s[i]-'0') * b + carry;
		s[i] = char('0' + t % 10), carry = t / 10;
	}
	while (carry) {
		s.insert(s.begin(), char('0' + carry % 10));
		carry /= 10;
	}
	return normalize(s);
}

pair<string, unsigned> divide(string s, unsigned b) {
	ull rem = 0;
	for (char& c : s) {
		ull t = rem*10 + unsigned(c-'0');
		c = char('0' + t/b), rem = t % b;
	}
	return {normalize(s), unsigned(rem)};
}

template<int N> void check(const Big<N>& x, string s) {
	for (unsigned limb : x) assert(limb < x.base);
	s = wrap<N>(s);
	assert(x.toString() == s);
	assert(Big<N>(s) == x);
	assert(Big<N>("000" + s) == x);
}

template<int N> void stress() {
	string maximum(9*N, '9');
	Big<N> maxval(maximum);
	check<N>(Big<N>(), "0");
	check<N>(Big<N>(ULLONG_MAX), to_string(ULLONG_MAX));
	check<N>(maxval + 1, "0");
	check<N>(Big<N>() - 1, maximum);
	check<N>(-Big<N>(), "0");
	check<N>(-maxval, "1");
	// Carries/borrows at every decimal and limb boundary.
	rep(i,0,9*N+1) {
		string power = "1" + string(i, '0');
		string a = wrap<N>(power);
		Big<N> x(power);
		check<N>(x, a);
		check<N>(x - 1, combine<N>(a, "1", true));
		check<N>((x - 1) + 1, a);
	}
	vector<unsigned> small = {0, 1, 2, 9, 10,
		999999999, 1000000000, 1000000001, UINT_MAX};
	for (unsigned m : small) {
		check<N>(maxval * m, multiply(maximum, m));
		check<N>(Big<N>() * m, "0");
		if (!m) continue;
		auto [q, r] = divide(maximum, m);
		check<N>(maxval / m, q);
		assert(maxval % m == r);
		check<N>(Big<N>() / m, "0");
		assert(Big<N>() % m == 0);
	}
	rep(it,0,2000) {
		string inputA, inputB;
		int digits = 1 + int(rng() % (20*N));
		rep(j,0,digits) inputA += char('0' + rng() % 10);
		rep(j,0,9*N) inputB += char('0' + rng() % 10);
		string a = wrap<N>(inputA), b = wrap<N>(inputB);
		Big<N> x(inputA), y(inputB);
		check<N>(x, a);
		check<N>(y, b);
		check<N>(x + y, combine<N>(a, b));
		check<N>(x - y, combine<N>(a, b, true));
		check<N>(-x, combine<N>("0", a, true));
		Big<N> z = x;
		z += z;
		check<N>(z, combine<N>(a, a));
		z -= z;
		check<N>(z, "0");
		bool less = sz(a) != sz(b) ? sz(a) < sz(b) : a < b;
		assert((x < y) == less);
		assert((x == y) == (a == b));
		assert((x != y) == (a != b));
		assert((x <= y) == (less || a == b));
		assert((x > y) == (!less && a != b));
		assert((x >= y) == !less);
		unsigned m = it < sz(small) ? small[it]
			: unsigned(rng());
		check<N>(x * m, multiply(a, m));
		check<N>(m * x, multiply(a, m));
		z = x;
		z *= m;
		check<N>(z, multiply(a, m));
		if (!m) m = 1;
		auto [q, r] = divide(a, m);
		check<N>(x / m, q);
		assert(x % m == r);
		z = x;
		assert(z.div(m) == r);
		check<N>(z, q);
		assert(z*m + r == x);
		z = x;
		z /= m;
		check<N>(z, q);
	}
	// Chained arithmetic and long input beyond capacity.
	Big<N> fact = 1;
	string s = "1";
	rep(i,1,201) {
		fact *= unsigned(i);
		s = wrap<N>(multiply(s, unsigned(i)));
		check<N>(fact, s);
	}
	string longInput(100000, '9');
	check<N>(Big<N>(longInput), maximum);
	check<N>(Big<N>(string(100000, '0') + "42"), "42");
}

template<int N> void nativeStress() {
	// Up to three limbs: even multiplication by UINT_MAX
	// fits in the independent native 128-bit oracle.
	u128 mod = 1;
	rep(i,0,N) mod *= Big<N>::base;
	auto value = [](Big<N> x) {
		u128 a = 0;
		for (unsigned limb : x) a = a*x.base + limb;
		return a;
	};
	rep(it,0,10000) {
		Big<N> x, y;
		rep(i,0,N) {
			x[i] = unsigned(rng() % x.base);
			y[i] = unsigned(rng() % y.base);
		}
		u128 a = value(x), b = value(y);
		unsigned m = unsigned(rng()), d = m ? m : 1;
		assert(value(x + y) == (a+b) % mod);
		assert(value(x - y) == (a+mod-b) % mod);
		assert(value(-x) == (mod-a) % mod);
		assert(value(x * m) == a*m % mod);
		assert(value(x / d) == a/d);
		assert(x % d == a%d);
		ull small = rng();
		assert(value(Big<N>(small)) == u128(small) % mod);
	}
}

int main() {
	assert(Big<1>("0000").toString() == "0");
	assert(Big<1>("1000000000") == Big<1>());
	assert(Big<3>("1000000000000000001").toString() ==
		"1000000000000000001");
	stress<1>();
	stress<2>();
	stress<3>();
	stress<8>();
	nativeStress<1>();
	nativeStress<2>();
	nativeStress<3>();
	cout << "Tests passed!" << endl;
}
