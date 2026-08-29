/**
 * Author: me
 * Date: 2026-08-16
 * License: CC0
 * Source: me
 * Description: Partial-pivot reduced row echelon form of a
 *  (possibly rectangular) matrix. Returns \texttt{(rank, det)}.
 *  Over $\mathbb{R}$, or modulo a prime (pass \texttt{mod}).
 *  \texttt{det} is meaningful for square matrices. Use this when
 *  you need the RREF itself (rank, nullspace, row space).
 *  For $Ax=b$ or inversion, see SolveLinear / MatrixInverse.
 * Time: $O(n^2 m)$
 * Status: stress-tested, Library Checker matrix\_rank
 */
#pragma once

#include "../number-theory/euclid.h"

typedef vector<double> vd;
typedef vector<vd> vvd;
const double RREF_EPS = 1e-10;

pair<int, double> rref(vvd& a) {
	int n = sz(a), m = n ? sz(a[0]) : 0, r = 0;
	double det = 1;
	for (int c = 0; c < m && r < n; c++) {
		int j = r;
		rep(i,r+1,n)
			if (fabs(a[i][c]) > fabs(a[j][c])) j = i;
		if (fabs(a[j][c]) < RREF_EPS) continue;
		swap(a[j], a[r]);
		if (j != r) det *= -1;
		det *= a[r][c];
		double s = 1 / a[r][c];
		rep(k,0,m) a[r][k] *= s;
		rep(i,0,n) if (i != r) {
			double t = a[i][c];
			rep(k,0,m) a[i][k] -= t * a[r][k];
		}
		r++;
	}
	if (n == m && r < n) det = 0;
	return {r, det};
}

pair<int, ll> rref(vector<vector<ll>>& A, ll mod) {
	int n = sz(A), m = n ? sz(A[0]) : 0, r = 0;
	ll det = 1;
	rep(c,0,m) {
		if (r == n) break;
		int p = r;
		while (p < n && !A[p][c]) ++p;
		if (p == n) continue;
		if (p != r) {
			swap(A[p], A[r]);
			det = (mod - det) % mod;
		}
		det = det * A[r][c] % mod;
		ll x, y; euclid(A[r][c], mod, x, y);
		ll iv = (x % mod + mod) % mod;
		rep(j,0,m) A[r][j] = A[r][j] * iv % mod;
		rep(i,0,n) if (i != r && A[i][c]) {
			ll f = A[i][c];
			rep(j,0,m) A[i][j] =
				(A[i][j] - f * A[r][j] % mod + mod) % mod;
		}
		++r;
	}
	if (n != m || r < n) det = 0;
	return {r, det};
}
