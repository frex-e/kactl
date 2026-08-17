#include "../utilities/template.h"

#include "../../content/numerical/RREF.h"
#include "../../content/numerical/Determinant.h"
#include "../../content/numerical/SolveLinear.h"

bool isZero(double x) { return fabs(x) < 1e-8; }

void checkRrefForm(const vvd& a, int rank) {
	int n = sz(a), m = sz(a[0]);
	vi pivot(rank, -1);
	rep(r,0,rank) {
		int c = 0;
		while (c < m && isZero(a[r][c])) c++;
		assert(c < m);
		assert(fabs(a[r][c] - 1) < 1e-8);
		rep(i,0,n) if (i != r) assert(isZero(a[i][c]));
		if (r) assert(c > pivot[r - 1]);
		pivot[r] = c;
	}
	rep(r,rank,n) rep(c,0,m) assert(isZero(a[r][c]));
}

vvd randMat(int n, int m) {
	vvd a(n, vd(m));
	rep(i,0,n) rep(j,0,m) a[i][j] = rand() % 11 - 5;
	return a;
}

int main() {
	{
		vvd a = {{1, 0}, {0, 1}};
		auto [r, d] = rref(a);
		assert(r == 2);
		assert(fabs(d - 1) < 1e-8);
	}
	{
		vvd a = {{1, 2}, {2, 4}};
		auto [r, d] = rref(a);
		assert(r == 1);
		assert(fabs(d) < 1e-8);
	}
	{
		vvd a = {{0, 0}, {0, 0}};
		auto [r, d] = rref(a);
		assert(r == 0);
		assert(fabs(d) < 1e-8);
	}
	{
		vvd a = {{1, 2, 3}, {4, 5, 6}};
		auto [r, d] = rref(a);
		assert(r == 2);
		checkRrefForm(a, r);
	}

	rep(it,0,500) {
		int n = rand() % 6 + 1;
		int m = rand() % 6 + 1;
		vvd a = randMat(n, m);
		vvd a2 = a, a3 = a;
		auto [rank, detR] = rref(a);
		checkRrefForm(a, rank);

		vd b(n), x(m);
		int sl = solveLinear(a2, b, x);
		assert(sl == rank);

		if (n == m) {
			double d = det(a3);
			if (rank < n) assert(isZero(detR) && isZero(d));
			else assert(fabs(detR - d) < 1e-6 * max(1.0, fabs(d)));
		}
	}
	cout << "Tests passed!" << endl;
}
