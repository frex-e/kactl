/**
 * Author: me
 * Date: 2026-08-16
 * License: CC0
 * Source: mine.typ
 * Description: Partial-pivot reduced row echelon form of a
 *  (possibly rectangular) matrix. Returns (rank, det).
 *  det is meaningful for square matrices. Use this when you
 *  need the RREF itself (rank, nullspace, row space).
 *  For $Ax=b$ or inversion, see SolveLinear / MatrixInverse.
 * Time: $O(n^2 m)$
 * Status: stress-tested
 */
#pragma once

typedef vector<double> vd;
typedef vector<vd> vvd;
const double RREF_EPS = 1e-10;

pair<int, double> rref(vvd& a) {
	int n = sz(a), m = sz(a[0]), r = 0;
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
