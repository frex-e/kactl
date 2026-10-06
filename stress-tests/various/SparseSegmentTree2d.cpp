#include "../utilities/template.h"
#include "../../content/various/SparseSegmentTree2d.h"

void testDense(int n, int m) {
	Tree2D tr(n, m);
	vector<vector<ll>> grid(n, vector<ll>(m, Tree2D::unit));
	rep(it,0,3000) {
		if (rand() % 2) {
			int x = rand() % n, y = rand() % m;
			ll v = rand() % 201 - 100;
			if (it % 17 == 0) v = Tree2D::unit;
			tr.set(x, y, v);
			grid[x][y] = v;
		} else {
			int a = rand() % (n+1), b = rand() % (n+1);
			int c = rand() % (m+1), d = rand() % (m+1);
			if (a > b) swap(a, b);
			if (c > d) swap(c, d);
			ll exp = Tree2D::unit;
			rep(x,a,b) rep(y,c,d) exp = max(exp, grid[x][y]);
			const Tree2D& ct = tr;
			assert(ct.query(a, b, c, d) == exp);
		}
	}
	// Every point and rectangle, including empty rectangles.
	rep(a,0,n+1) rep(b,a,n+1) rep(c,0,m+1) rep(d,c,m+1) {
		ll exp = Tree2D::unit;
		rep(x,a,b) rep(y,c,d) exp = max(exp, grid[x][y]);
		assert(tr.query(a, b, c, d) == exp);
	}
}

void testOverwrite() {
	Tree2D tr(5, 7);
	assert(tr.query(0, 5, 0, 7) == Tree2D::unit);
	tr.set(0, 3, 20);
	tr.set(4, 3, 10);
	tr.set(0, 3, -20);
	assert(tr.query(0, 5, 0, 7) == 10);
	assert(tr.query(0, 1, 3, 4) == -20);
	assert(tr.query(1, 4, 3, 4) == Tree2D::unit);
	tr.set(4, 3, Tree2D::unit);
	assert(tr.query(0, 5, 0, 7) == -20);
	tr.set(0, 3, Tree2D::unit);
	assert(tr.query(0, 5, 0, 7) == Tree2D::unit);
}

void testLargeDomain() {
	const int N = INT_MAX, M = 1'000'000'007;
	Tree2D tr(N, M);
	map<pii, ll> points;
	assert(tr.query(0, N, 0, M) == Tree2D::unit);
	// An empty tree must remain empty after queries.
	assert(!tr.root.lt && !tr.root.rt);
	assert(!tr.root.tree.lt && !tr.root.tree.rt);
	vector<pii> coords = {{0, 0}, {N-1, M-1},
		{0, M-1}, {N-1, 0}, {N/2, M/2}};
	rep(i,0,200) coords.emplace_back(rand() % N, rand() % M);
	rep(it,0,3000) {
		if (it < sz(coords) || rand() % 2) {
			auto [x, y] = coords[it % sz(coords)];
			ll v = (ll)rand() * rand();
			if (it % 2) v = -v;
			if (it % 13 == 0) v = Tree2D::unit;
			tr.set(x, y, v);
			points[{x, y}] = v;
			assert(tr.query(x, x+1, y, y+1) == v);
		} else {
			int a = rand() % N, b = rand() % N;
			int c = rand() % M, d = rand() % M;
			if (a > b) swap(a, b);
			if (c > d) swap(c, d);
			if (it % 5 == 0) a = c = 0, b = N, d = M;
			ll exp = Tree2D::unit;
			for (auto [p, v] : points) {
				auto [x, y] = p;
				if (a <= x && x < b && c <= y && y < d)
					exp = max(exp, v);
			}
			assert(tr.query(a, b, c, d) == exp);
		}
	}
}

int main() {
	srand(123456);
	testOverwrite();
	rep(n,1,10) rep(m,1,10) testDense(n, m);
	testLargeDomain();
	cout << "Tests passed!" << endl;
}
