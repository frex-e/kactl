#include "../utilities/template.h"

#define pb push_back
#include "../../content/strings/Duval.h"
#include "../../content/strings/MinRotation.h"

// Exact CP-Algorithms implementation (returns the Lyndon words).
vector<string> duvalOrig(string const& s) {
	int n = sz(s);
	int i = 0;
	vector<string> factorization;
	while (i < n) {
		int j = i + 1, k = i;
		while (j < n && s[k] <= s[j]) {
			if (s[k] < s[j])
				k = i;
			else
				k++;
			j++;
		}
		while (i <= k) {
			factorization.push_back(s.substr(i, j - k));
			i += j - k;
		}
	}
	return factorization;
}

bool isLyndon(const string& w) {
	int n = sz(w);
	if (!n) return false;
	rep(i,1,n)
		if (w.compare(0, n, w, i, n - i) >= 0) return false;
	return true;
}

void checkCFL(const string& s, const vi& f) {
	assert(!f.empty() && f[0] == 0 && f.back() == sz(s));
	rep(i,0,sz(f)-1) assert(f[i] < f[i+1]);
	vector<string> w;
	rep(i,0,sz(f)-1) {
		w.push_back(s.substr(f[i], f[i+1] - f[i]));
		assert(isLyndon(w.back()));
	}
	rep(i,0,sz(w)-1) assert(w[i] >= w[i+1]);
}

void test(const string& s) {
	vi f = duval(s);
	checkCFL(s, f);

	vector<string> orig = duvalOrig(s);
	assert(sz(orig) + 1 == sz(f));
	rep(i,0,sz(orig))
		assert(orig[i] == s.substr(f[i], f[i+1] - f[i]));

	if (!s.empty()) {
		string a = s, b = s;
		int r = minRotation(a);
		rotate(a.begin(), a.begin() + r, a.end());
		string t = s + s;
		vi g = duval(t);
		int n = sz(s), pos = 0;
		rep(i,0,sz(g)-1)
			if (g[i] < n && g[i+1] >= n) pos = g[i];
		rotate(b.begin(), b.begin() + pos, b.end());
		assert(a == b);
	}
}

template<class F>
void gen(string& s, int at, int alpha, F f) {
	if (at == sz(s)) f();
	else {
		rep(i,0,alpha) {
			s[at] = (char)('a' + i);
			gen(s, at + 1, alpha, f);
		}
	}
}

int main() {
	assert(duval("") == vi{0});
	assert((duval("babaabaab") == vi{0, 1, 3, 6, 9}));
	assert((duval("ababacaca") == vi{0, 8, 9}));
	assert((duval("banana") == vi{0, 1, 3, 5, 6}));
	assert((duval("aababab") == vi{0, 5, 7}));

	rep(n,0,13) {
		string s(n, 'x');
		gen(s, 0, 3, [&]() { test(s); });
	}
	rep(n,0,11) {
		string s(n, 'x');
		gen(s, 0, 4, [&]() { test(s); });
	}

	srand(1);
	rep(it,0,10000) {
		int n = rand() % 40;
		int alpha = 1 + rand() % 6;
		string s;
		rep(i,0,n) s += (char)('a' + rand() % alpha);
		test(s);
	}
	rep(it,0,200) {
		int n = 200 + rand() % 50;
		string s;
		rep(i,0,n) s += (char)('a' + rand() % 2);
		test(s);
	}

	cout << "Tests passed!" << endl;
}
