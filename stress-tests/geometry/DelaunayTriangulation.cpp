#include "../utilities/template.h"

#include "../../content/geometry/DelaunayTriangulation.h"
#include "../../content/geometry/ConvexHull.h"
#include "../../content/geometry/PolygonArea.h"
#include "../../content/geometry/circumcircle.h"

typedef complex<double> P;
int main() {
	#ifdef __GLIBC__
	feenableexcept(29);
	#endif
	rep(it,0,100000) {{
		vector<P> ps;
		int N = rand() % 20 + 1;
		rep(i,0,N) {
			ps.emplace_back(rand() % 100 - 50, rand() % 100 - 50);
		}

		auto coc = [&](int i, int j, int k, int l) {
			double a = dist(ps[i] - ps[j]);
			double b = dist(ps[j] - ps[k]);
			double c = dist(ps[k] - ps[l]);
			double d = dist(ps[l] - ps[i]);
			double e = dist(ps[i] - ps[k]);
			double f = dist(ps[j] - ps[l]);
			double q = a*c + b*d - e*f;
			return abs(q) < 1e-4;
		};

		rep(i,0,N) rep(j,0,i) rep(k,0,j) {
			if (orient(ps[i], ps[j], ps[k]) == 0) {  goto fail; }
		}
		rep(i,0,N) rep(j,0,i) rep(k,0,j) rep(l,0,k) {
			if (coc(i,j,k,l) || coc(i,j,l,k) || coc(i,l,j,k) || coc(i,l,k,j)) { goto fail; }
		}

		auto fail = [&]() {
			cout << "Points:" << endl;
			for(auto &p: ps) {
				cout << p.real() << ' ' << p.imag() << endl;
			}

			cout << "Triangles:" << endl;
			delaunay(ps, [&](int i, int j, int k) {
				cout << i << ' ' << j << ' ' << k << endl;
			});

			abort();
		};

		double sumar = 0;
		vi used(N);
		delaunay(ps, [&](int i, int j, int k) {
			used[i] = used[j] = used[k] = 1;
			double ar = orient(ps[i], ps[j], ps[k]);
			if (ar < -1e-4) fail();
			sumar += ar;
			P c = ccCenter(ps[i], ps[j], ps[k]);
			double ra = ccRadius(ps[i], ps[j], ps[k]);
			rep(l,0,N) {
				if (dist(ps[l] - c) < ra - 1e-5) fail();
			}
		});
		if (N >= 3) rep(i,0,N) if (!used[i]) fail();

		vector<P> hull = convexHull(ps);
		double ar2 = polygonArea2(hull);
		if (abs(sumar - ar2) > 1e-4) fail();

		continue; }
fail:;
	}
	cout<<"Tests passed!"<<endl;
}
