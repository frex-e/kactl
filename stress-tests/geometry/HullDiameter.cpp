#include "../utilities/template.h"

#include "../../content/geometry/ConvexHull.h"
typedef pp P;
#include "../../content/geometry/HullDiameter.h"

int main() {
	srand(2);
	rep(it,0,1000000) {
		int N = (rand() % 10) + 1;
		vector<P> ps;
		rep(i,0,N) {
			ps.emplace_back(rand() % 11 - 5, rand() % 11 - 5);
		}
		double r1 = 0;
		rep(i,0,N) rep(j,0,i) {
			r1 = max(r1, norm(ps[i] - ps[j]));
		}
		auto pa = hullDiameter(convexHull(ps));
		double r2 = ps.empty() ? 0 : norm(pa[0] - pa[1]);
		assert(r1 == r2);
	}
	cout<<"Tests passed!"<<endl;
}
