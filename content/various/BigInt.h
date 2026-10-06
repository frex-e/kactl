/**
 * Author: caterpillow, Claude, adapted locally
 * Date: 2026-10-06
 * License: CC0
 * Source: https://github.com/caterpillow/cactl
 * Description: Unsigned integer with $N>0$ base-$10^9$
 *  limbs, most significant first (up to $9N$ digits).
 *  Arithmetic and parsing wrap modulo $10^{9N}$.
 *  String input must be nonempty decimal digits; output
 *  has no leading zeros. Multiply/divide by an unsigned
 *  32-bit integer; division requires a nonzero divisor.
 *  \texttt{div(b)} divides in place and returns the remainder.
 * Usage: Big<4> x("12345678901234567890"); x.toString();
 * Time: $O(N)$ arithmetic, $O(N+D)$ string conversion
 *  for $D$ decimal digits.
 * Status: stress-tested
 */
#pragma once

using ull = unsigned long long;
template<int N> struct Big : array<unsigned, N> {
	static_assert(N > 0);
	static constexpr unsigned base = 1000000000;
	Big(ull x = 0) : array<unsigned, N>{} {
		for (int i = N; i-- && x; x /= base)
			(*this)[i] = unsigned(x % base);
	}
	explicit Big(string_view s) : Big() {
		assert(!s.empty());
		for (char c : s) assert('0' <= c && c <= '9');
		int end = sz(s);
		for (int i = N; i-- && end > 0;) {
			int start = max(0, end-9);
			unsigned x = 0;
			rep(j,start,end) x = x*10 + unsigned(s[j]-'0');
			(*this)[i] = x, end = start;
		}
	}
	string toString() const {
		int i = 0;
		while (i < N-1 && !(*this)[i]) i++;
		string s = to_string((*this)[i]);
		while (++i < N) {
			string t = to_string((*this)[i]);
			s += string(9-sz(t), '0') + t;
		}
		return s;
	}
	Big& operator+=(const Big& b) {
		unsigned carry = 0;
		for (int i = N; i--;) {
			unsigned t = (*this)[i] + b[i] + carry;
			(*this)[i] = t % base, carry = t / base;
		}
		return *this;
	}
	Big operator-() const {
		Big r = *this;
		for (unsigned& x : r) x = base-1-x;
		return r += 1;
	}
	Big& operator-=(const Big& b) { return *this += -b; }
	friend Big operator+(Big a, const Big& b) { return a += b; }
	friend Big operator-(Big a, const Big& b) { return a -= b; }
	Big& operator*=(unsigned b) {
		ull carry = 0;
		for (int i = N; i--;) {
			ull t = ull((*this)[i]) * b + carry;
			(*this)[i] = unsigned(t % base), carry = t / base;
		}
		return *this;
	}
	unsigned div(unsigned b) {
		assert(b);
		ull rem = 0;
		rep(i,0,N) {
			ull t = rem * base + (*this)[i];
			(*this)[i] = unsigned(t / b), rem = t % b;
		}
		return unsigned(rem);
	}
	Big& operator/=(unsigned b) { div(b); return *this; }
	friend Big operator*(Big a, unsigned b) { return a *= b; }
	friend Big operator*(unsigned b, Big a) { return a *= b; }
	friend Big operator/(Big a, unsigned b) { return a /= b; }
	friend unsigned operator%(Big a, unsigned b) {
		return a.div(b);
	}
};
