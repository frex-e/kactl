#import "@preview/codly:1.3.0": *
#set page(columns: 3, flipped: true, margin: 1cm)
#set text(size: 0.6em)

#show: codly-init.with()
// No line numbers (for nice digital copy-pasting)
#codly(number-format: none, display-name: false)

// TODO 
// - Some large primes for hashing
// - Large highly composite numbers for testing bounds
// - random value apis
// - 2d query structures

= Preamble
```cpp
#include <bits/stdc++.h>
using namespace std;
#define rep(i, a, b) for(int i = a; i < (b); ++i)
#define all(x) begin(x), end(x)
#define sz(x) (int)(x).size()
#define pb push_back
#define fr first
#define sc second
typedef long long ll;
typedef pair<int, int> pii;
typedef vector<int> vi;

int main() {
	cin.tie(0)->sync_with_stdio(0);
}
```

= Vim
```vim
set smartindent
syntax on
set tabstop=2
set shiftwidth=2
set number
set smartcase
set incsearch
set hlsearch

" optional
set mouse=a
set clipboard=unnamed #plus

```

== Debug Memory Usage
```cpp
ll get_memory_usage() {
	struct rusage usage;
	getrusage(RUSAGE_SELF, &usage);
	return usage.ru_maxrss; // Maximum resident set size (in kilobytes on Linux, bytes on macOS)
}
```

== Output
```cpp
// Fixed precision.
cout << fixed << setprecision(6) << lf << '\n';
// Binary output
cout << format("{:06b}", b) << "fixed length binary";
cout << format("{:b}", b) << "variable length binary";
```

== Other Things to Know
```cpp
__builtin_ctz(i); // Trailing Zeroes ints
__builtin_clzll(i); // Leading from long long
__builtin_popcountll(i); // number of ones
```


= Number Theory
== Mobius Function
See KACTL for info.
```
mobius[1] = -1;
for (int i = 1; i < VALMAX; i++) {
	if (mobius[i]) {
		mobius[i] = -mobius[i];
		for (int j = 2 * i; j < VALMAX; j += i) { mobius[j] += mobius[i]; }
	}
}
```

== Linear Prime Sieve
This calculates the minimum prime factor `pr[j]` for all all $j$ up to $n$.
From this, we can calculate the prime factorisation of all these numbers. \
*Time: * $cal(O)(n)$

```cpp
const int N = 10000000;
vector<int> lp(N+1);
vector<int> pr;
for (int i=2; i <= N; ++i) {
	if (lp[i] == 0) { lp[i] = i; pr.push_back(i); }
	for (int j = 0; i * pr[j] <= N; ++j) {
		lp[i * pr[j]] = pr[j];
		if (pr[j] == lp[i]) break;
	}
}
```

= String Matching
== Random Hash Base
```cpp
mt19937_64 rng(chrono::steady_clock::now().time_since_epoch().count());
uint64_t base = uniform_int_distribution<uint64_t>(256, (1ULL << 61) - 2)(rng);
base |= 1ULL;
```

== Suffix Array for String Comp
```cpp
struct SuffixArray {
	vi sa, lcp, rank, lg;
	vector<vi> st;
	SuffixArray(string s, int lim = 256) { // or vector<int>
		s.push_back(0);
		int n = sz(s), k = 0, a, b;
		vi x(all(s)), y(n), ws(max(n, lim));
		sa = lcp = y, iota(all(sa), 0);
		for (int j = 0, p = 0; p < n; j = max(1, j * 2), lim = p) {
			p = j, iota(all(y), n - j);
			rep(i,0,n) if (sa[i] >= j) y[p++] = sa[i] - j;
			fill(all(ws), 0);
			rep(i,0,n) ws[x[i]]++;
			rep(i,1,lim) ws[i] += ws[i - 1];
			for (int i = n; i--;) sa[--ws[x[y[i]]]] = y[i];
			swap(x, y), p = 1, x[sa[0]] = 0;
			rep(i,1,n) {
				a = sa[i - 1], b = sa[i];
				x[b] = (y[a] == y[b] && y[a + j] == y[b + j]) ? p - 1 : p++;
			}
		}
		for (int i = 0, j; i < n - 1; lcp[x[i++]] = k)
			for (k && k--, j = sa[x[i] - 1];
					s[i + k] == s[j + k]; k++);
		rank = x;      // inverse suffix array
		buildRMQ();
	}

	void buildRMQ() {
		int n = sz(lcp);
		lg.assign(n + 1, 0);
		rep(i,2,n+1) lg[i] = lg[i/2] + 1;
		st.assign(lg[n] + 1, vi(n));
		st[0] = lcp;
		rep(k,1,sz(st)) rep(i,0,n - (1<<k) + 1)
			st[k][i] = min(st[k-1][i], st[k-1][i + (1<<(k-1))]);
	}

	int rmq(int l, int r) { // inclusive
		if (l > r) return INT_MAX;
		int k = lg[r - l + 1];
		return min(st[k][l], st[k][r - (1<<k) + 1]);
	}

	int getLCP(int i, int j) {
		if (i == j) return sz(sa) - 1 - i; // ignore added sentinel
		int ri = rank[i], rj = rank[j];
		if (ri > rj) swap(ri, rj);
		return rmq(ri + 1, rj);
	}

	// compare s[a..a+lena) vs s[b..b+lenb)
	// returns -1 / 0 / 1
	int cmpSubstr(int a, int lena, int b, int lenb, const string& s) {
		int common = min(getLCP(a, b), min(lena, lenb));
		if (common == min(lena, lenb)) {
			if (lena == lenb) return 0;
			return lena < lenb ? -1 : 1;
		}
		return s[a + common] < s[b + common] ? -1 : 1;
	}
};
```

= Structures
== Binary Trie
```cpp
#define ull unsigned long long
struct BinaryTrie {
  BinaryTrie *nxt[2];
  ull size = 0; // Size only needed for mex
  BinaryTrie() {
		nxt[0] = nullptr;
		nxt[1] = nullptr;
	}
	bool insert(ull x, int ind = 31) {
		if (ind < 0) {
			if (size == 0) return size = 1;
				return false;
			}
		ull bit = 1 & (x >> ind);
		if (nxt[bit] == nullptr) nxt[bit] = new BinaryTrie();
		bool res = nxt[bit]->insert(x,ind - 1);
		size += res;
		return res;
	}
	ull mex(ull xr, ull cur = 0ull, int ind = 31) {
		if (ind < 0) return cur;
		ull bit = (xr >> ind) & 1;
		if (nxt[bit] and nxt[bit]->size == (1ull << ind)) {
			if (nxt[!bit]) return nxt[!bit]->mex(xr, cur | (1ull << ind), ind - 1);
			return cur | (1ull << ind);
		}
		else if (nxt[bit]) return nxt[bit]->mex(xr, cur, ind - 1);
		return cur;
	}
	ull max(ull xr, ull cur = 0ull, int ind = 31) {
		if (ind < 0) return cur;
		ull bit = (xr >> ind) & 1;
		if (nxt[!bit])
			return nxt[!bit]->max(xr, cur | (1ull << ind), ind - 1);
		if (nxt[bit])
			return nxt[bit]->max(xr,cur,ind - 1);
		// empty case
	}
	~BinaryTrie() {
		if (nxt[0]) delete nxt[0];
		if (nxt[1]) delete nxt[1];
	}
};
```

= Segment Trees!!!
== Basic
```cpp
struct BasicSegmentTree {
	using V = int;
	V id = INT_MAX;
	V binop(V a, V b) {return min(a, b);}
	vector<V> arr;
	int size;
	BasicSegmentTree(int n) : arr(4*n + 2,id), size(n) {};
	void update(int cur, int l, int r, int i, V v) {
		if (i < l or r < i) return;
		if (l == r and i == r) { arr[cur] = v; return; }
		int mid = midpoint(l, r);
		update(2*cur, l, mid, i, v);
		update(2*cur+1, mid+1, r, i, v);
		arr[cur] = binop(arr[2*cur],arr[2*cur+1]);
	}
	void update(int i, int v) {update(1,0,size-1,i,v);}
	V query(int cur, int l, int r, int ql, int qr) {
		if (qr < l or r < ql) return id;
		if (ql <= l and r <= qr) return arr[cur];
		int mid = midpoint(l,r);
		return binop(query(2*cur, l, mid, ql, qr), query(2*cur + 1, mid + 1, r, ql, qr));
	}
	V query(int ql, int qr) {return query(1,0,size - 1,ql,qr);}
};
```

== Lazy Update
```cpp
struct LazyUpdateTree {
	using V = int;
	using U = int;
	const V id = INT_MIN;
	const V def = 0;
	const U idU = 0;
	V binop(V a, V b) { return max(a, b); }
	V applyUpdate(U u, V v) { return v + u; }
	U mergeUpdate(U old, U nw) { return old + nw; }
	vector<V> arr;
	vector<U> lazy;
	int size;
	LazyUpdateTree(int n) : 
		arr(4*n+2, def), 
		lazy(4*n+2, idU), 
		size(n) {};
	void updateNode(int cur, int l, int r, U u) {
		lazy[cur] = mergeUpdate(lazy[cur], u);
		arr[cur] = applyUpdate(u, arr[cur]);
	}
	void push(int cur, int l, int r) {
		int mid = midpoint(l, r);
		updateNode(2*cur,l,mid,lazy[cur]);
		updateNode(2*cur+1,mid+1,r,lazy[cur]);
		lazy[cur] = idU;
	}
	void update(int cur, int l, int r, int ql, int qr, U u) {
		if (qr < l or r < ql) return;
		if (ql <= l and r <= qr) {
			updateNode(cur,l,r,u); return;
		}
		push(cur, l, r);
		int mid = midpoint(l, r);
		update(2 * cur, l, mid, ql, qr, u);
		update(2 * cur + 1, mid + 1, r, ql, qr, u);
		arr[cur] = binop(arr[2 * cur], arr[2 * cur + 1]);
	}
	void update(int ql, int qr, U u) { update(1, 0, size - 1, ql, qr, u); }

	V query(int cur, int l, int r, int ql, int qr) {
		if (qr < l or r < ql) return id;
		if (ql <= l and r <= qr) return arr[cur];
		push(cur, l, r);
		int mid = midpoint(l, r);
		return binop(
			query(2 * cur, l, mid, ql, qr), 
			query(2 * cur + 1, mid + 1, r, ql, qr)
			);
	}
	V query(int ql, int qr) { return query(1, 0, size - 1, ql, qr); }
};
```

== Lazy Create Segment Tree
```cpp
struct Node {
	using V = int;
	static const V id = 0;
	V binop(V a, V b) { return a + b; }
	V val = id;
	Node *lt = NULL, *rt = NULL;
	Node() {};
	void push() {
		if (!lt) { lt = new Node(); rt = new Node(); }
	}
	void update(int l, int r, int i, V v) {
		if (i < l or r < i) return;
		if (l == r and r == i) { val = v; return; }
		push();
		int mid = midpoint(l, r);
		lt->update(l,mid,i,v);
		rt->update(mid+1,r,i,v);
		val = binop(lt->val,rt->val);
	}
	V query(int l, int r, int ql, int qr) {
		if (qr < l or r < ql) return id;
		if (ql <= l and r <= qr) return val;
		push();
		int mid = midpoint(l, r);
		return binop(
			lt->query(l, mid, ql, qr), 
			rt->query(mid + 1, r, ql, qr));
	}
	~Node() {delete lt; delete rt;}
};
```

== Lazy Create Update
```cpp
struct Node {
	using V = int; using U = int;
	static const V id = INT_MIN;
	static const V def = 0;
	static const U idU = 0;
	V binop(V a, V b) { return max(a, b); }
	V applyUpdate(U u, V v) { return v + u; }
	U mergeUpdate(U old, U nw) { return old + nw; }
	Node *lt = nullptr, *rt = nullptr;
	V val = def;
	U lazy = idU;
	void updateNode (int l, int r, U u) {
		lazy = mergeUpdate(lazy,u);
		val = applyUpdate(u,val);
	}
	void push(int l, int r) {
		if (!lt) { lt = new Node(); rt = new Node(); }
		int mid = midpoint(l, r);
		lt->updateNode(l,mid,lazy);
		rt->updateNode(mid+1,r,lazy);
		lazy = idU;
	}
	void update(int l, int r, int ql, int qr, U u) {
		if (qr < l or r < ql) return;
		if (ql <= l and r <= qr) {
			updateNode(l,r,u);
			return;
		}
		push(l, r);
		int mid = midpoint(l, r);
		lt->update(l, mid, ql, qr, u);
		rt->update(mid + 1, r, ql, qr, u);
		val = binop(lt->val, rt->val);
	}
	V query(int l, int r, int ql, int qr) {
		if (qr < l or r < ql) return id;
		if (ql <= l and r <= qr) return val;
		push(l, r);
		int mid = midpoint(l, r);
		return binop(lt->query(l, mid, ql, qr), rt->query(mid + 1, r, ql, qr));
	}
	~Node() {delete lt; delete rt;}
};
```

= DP Optimisations

// TODO Li Chao on Floats
== Li Chao Tree
```cpp
using ll = long long;
struct Function {
  ll a, b;
  Function(ll a = 0, ll b = 0) : a(a), b(b) {}
  ll f(ll x) const { return a * x + b; }
};
const ll INF = 1e18;
struct Node {
  Function function{0, INF};
  Node *left = nullptr, *right = nullptr;
};
Node *root = new Node();
const int S = 350, L = S + 1, R = 1e5 + 1;
void insertFunction(Function newFunction, Node *node = root, int l = L, int r = R) {
  if (!node) return;
  int m = (l + r) >> 1;
  bool left = newFunction.f(l) < node->function.f(l);
  bool mid = newFunction.f(m) < node->function.f(m);
  if (mid) swap(newFunction, node->function);
  if (l == r - 1) return;
  Node *&child = left != mid ? node->left : node->right;
  if (!child) child = new Node();
  insertFunction(newFunction, child, left != mid ? l : m, left != mid ? m : r);
}
ll query(int x, Node *node = root, int l = L, int r = R) {
  if (!node || l > x || r <= x) return INF;
  int m = (l + r) >> 1;
  ll res = node->function.f(x);
  return x < m
		? min(res, query(x, node->left, l, m))
		: min(res, query(x, node->right, m, r));
}
```

== Quadrangle Inequality
$C(a,c) + C(b,d) <= C(a,d) + C(b, c)$

"Wider gets worser faster"

Patterns:
+ Show that $C(i,j) + C(i+1,j+1) <= C(i,j+1) + C(i+1,j)$
+ $C(i,j) = sum_(k = i)^j w_k$
+ $C(i,j) = g(x(j) - x(i))$ where $x$ is increasing and $g$ is convex.
+ $C(i,j) = min(A[i], B[j])$ or (max)
+ $C(i,j) = max(0, x_j - x_i - Delta)$ where $Delta > 0$
// TODO double check above.
== Divide and Conquer
For recurrences of the form:
$ "dp"(i,j) = min_(0 <= k <= j) "dp"(i - 1, k - 1) + C(k,j) $

where the splitting point increases as j increases. Quadrangle
is sufficient.
```cpp
// Inclusive, inclusive, 0-indexed
auto cost = [&](int i, int j) -> double {
	return 0;
};
vector<double> dpbefore(n, 1./0.);
vector<double> dpafter(n, 1./0.);
auto compute = [&](const auto& self, int l, int r, int optl, int optr) -> void {
	if (l > r) return;
	int mid = midpoint(l,r);
	pair<double, int> best = {1./0., -1};
	rep(k, optl, min(mid,optr) + 1) {
		double cur = (k ? dpbefore[k - 1] : 0) + cost(k, mid);
		best = min(best, {cur,k});
	}
	dpafter[mid] = best.first;
	self(self, l, mid - 1, optl, best.second);
	self(self, mid + 1, r, best.second, optr);
};
rep(a,0,k) {
	compute(compute, 0, n - 1, 0, n - 1);
	swap(dpbefore, dpafter);
}
cout << dpbefore.back();
```
== Knuth's Optimisation
#let dp = "dp"
Any fixed offsets from k work.
$dp(i, j) = min_(i<=k<j)(dp(i, k) + dp(k + 1, j)) + C(i,j)$

$"opt"(i,j) = "optimal selection of k"$
Requires that: $"opt"(i, j - 1) <= "opt"(i,j) <= "opt"(i+1,j)$.

Following is sufficient:
+ $C(b,c) <= C(a,d)$
+ Quadrangle Inequality

```cpp
int dp[N][N], opt[N][N];
auto C = [&](int i, int j) {
	// Implement cost function C.
};
for (int i = 0; i < N; i++) {
	opt[i][i] = i;
	// Initialize dp[i][i] according to the problem
}
for (int i = N-2; i >= 0; i--) {
	for (int j = i+1; j < N; j++) {
		int mn = INT_MAX;
		int cost = C(i, j);
		for (int k = opt[i][j-1]; k <= min(j-1, opt[i+1][j]); k++) {
			if (mn >= dp[i][k] + dp[k+1][j] + cost) {
				opt[i][j] = k;
				mn = dp[i][k] + dp[k+1][j] + cost;
			}
		}
		dp[i][j] = mn;
	}
}
return dp[0][N-1];
```


= Geometry
=== Preamble
```cpp
typedef complex<long long> point;
#define x real()
#define y imag()
inline ll dot(point a, point b) { return (conj(a)*b).x; }
inline ll cross(point a, point b) { return (conj(a) * b).y; }
inline ll dist2(point a, point b) { return dot(a - b, a-b); }
inline ll orient(point a, point b, point c) { return cross(b - a, c - a); } // Positive = CCW
```

== Johnson's
- Directed edges
- Bellman-Ford with every vertex starting at distance 0.
- (Check for negative cycles).
- Modify edge like so: $w'(u,v) = w(u,v) + h(u) - h(v)$.
- Run Dijkstra's from each source node.


= Linear Algebra
== Gauss-Jordan
=== Partial Pivot RREF - Rectangular
```cpp
const double EPSILON = 1e-10;
typedef double T;
typedef vector<T> VT;
typedef vector<VT> VVT;
tuple<int,double> rref(VVT &a) {
  int n = a.size();
  int m = a[0].size();
  int r = 0;
  double det = 1.;
  for (int c = 0; c < m && r < n; c++) {
	int j = r;
	for (int i = r + 1; i < n; i++)
	  if (fabs(a[i][c]) > fabs(a[j][c])) j = i;
	if (fabs(a[j][c]) < EPSILON) continue;
	swap(a[j], a[r]);
	if (j != r) det *= -1.;
	det *= a[r][c];
	T s = 1.0 / a[r][c];
	for (int j = 0; j < m; j++) a[r][j] *= s;
	for (int i = 0; i < n; i++) if (i != r) {
	  T t = a[i][c];
	  for (int j = 0; j < m; j++) a[i][j] -= t * a[r][j];
	}
	r++;
  }
  if (n == m && r < n) det = 0;
  return {r,det};
}
```
=== Full Pivot - Inverse, Square, Solving $(n times n) dot (n times m) = (n . times m)$
- Solving systems of linear equations ($A X = B$)
- Inverting matrices ($A X= I$)
- Computing determinants of square matrices
Runs in $cal(O)(n^3)$

Output:
- $X$ stored in `b`
- $A^(-1)$ stored in `a`
```cpp
const double EPS = 1e-10;
typedef vector<int> VI;
typedef double T;
typedef vector<T> VT;
typedef vector<VT> VVT;
T GaussJordan(VVT &a, VVT &b) {
  const int n = a.size();
  const int m = b[0].size();
  VI irow(n), icol(n), ipiv(n);
  T det = 1;
  for (int i = 0; i < n; i++) {
	int pj = -1, pk = -1;
	for (int j = 0; j < n; j++) if (!ipiv[j])
	  for (int k = 0; k < n; k++) if (!ipiv[k])
	if (pj == -1 || fabs(a[j][k]) > fabs(a[pj][pk])) { pj = j; pk = k; }
	if (fabs(a[pj][pk]) < EPS) { cerr << "Matrix is singular." << endl; exit(0); }
	ipiv[pk]++;
	swap(a[pj], a[pk]);
	swap(b[pj], b[pk]);
	if (pj != pk) det *= -1;
	irow[i] = pj;
	icol[i] = pk;
	T c = 1.0 / a[pk][pk];
	det *= a[pk][pk];
	a[pk][pk] = 1.0;
	for (int p = 0; p < n; p++) a[pk][p] *= c;
	for (int p = 0; p < m; p++) b[pk][p] *= c;
	for (int p = 0; p < n; p++) if (p != pk) {
	  c = a[p][pk];
	  a[p][pk] = 0;
	  for (int q = 0; q < n; q++) a[p][q] -= a[pk][q] * c;
	  for (int q = 0; q < m; q++) b[p][q] -= b[pk][q] * c;
	}
  }
  for (int p = n-1; p >= 0; p--) if (irow[p] != icol[p]) {
	for (int k = 0; k < n; k++) swap(a[k][irow[p]], a[k][icol[p]]);
  }
  return det;
}
```

== XOR Basis
Small vectors
```cpp
vector<int> basis;
void add(int x) {
	for (int i = 0; i < basis.size(); i++) {  // reduce x using the current basis vectors
		x = min(x, x ^ basis[i]);
	}
	if (x != 0) { basis.push_back(x); }
}
```

Arbitrarily large vectors
```cpp
bool non_zero(const vector<uint64_t>& x) {
	bool non_zero = false;
	for(const auto& a : x) {
		non_zero |= (a != (uint64_t) 0);
	}
	return non_zero;
}
struct Basis {
	vector<vector<uint64_t>> basis;
	vector<uint64_t> reduce(vector<uint64_t> x) {
		for(int i = 0; i < basis.size(); i++) {
			int state = 0;
			for(int j = 0; j < x.size(); j++) {
				uint64_t cur = basis[i][j] ^ x[j];
				if (state == 0 and cur < x[j]) state = -1;
				if (state == 0 and cur > x[j]) state = 1;
				if (state <= 0) x[j] = cur;
			}
		}
		return x;
	}
	void add(vector<uint64_t> x) {
		x = reduce(x);
		if (non_zero(x)) basis.push_back(x);
	}
	bool equal(const Basis& other) {
		if (other.basis.size() != basis.size()) return false;
		bool ans = true;
		for(const auto & v : other.basis) {
			ans &= !non_zero(reduce(v));
		}
		return ans;
	}
};
```

== Pragmas
```cpp
#pragma GCC optimize("Ofast,unroll-loops")
#pragma GCC target("avx2,bmi,bmi2,lzcnt,popcnt")
```