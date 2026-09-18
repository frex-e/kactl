#include "../utilities/template.h"

#include "../../content/geometry/CircleIntersection.h"

int main() {
	cin.sync_with_stdio(0); cin.tie(0);
	cin.exceptions(cin.failbit);
	srand(2);
	rep(it,0,100000) {
		double rnd[6];
		rep(i,0,6)
			rnd[i] = rand() % 21 - 10;
		P a(rnd[0], rnd[1]);
		P b(rnd[2], rnd[3]);
		double ra = rand() % 10;
		double rb = rand() % 10;
		if (a == b) continue;
		pair<P, P> out;
		bool ret = circleInter(a, b, ra, rb, &out);
		if (ret) {
			assert(abs(dist(out.first - a) - ra) < 1e-9);
			assert(abs(dist(out.second - a) - ra) < 1e-9);
			assert(abs(dist(out.first - b) - rb) < 1e-9);
			assert(abs(dist(out.second - b) - rb) < 1e-9);
		}

		// Hill-climb the answer
		auto func = [&](P x) {
			double d1 = dist(x - a) - ra;
			double d2 = dist(x - b) - rb;
			return d1*d1 + d2*d2;
		};
		P start = (a + b) / 2.0 + perp(a - b);
		pair<double, P> cur(func(start), start);
		for (double jmp = 100; jmp > 1e-20; jmp /= 2) {
			int iters = 0;
			for (int imp = 1; imp--;) {
				if (++iters == 100) goto skip;
				rep(dx,-1,2) rep(dy,-1,2) {
					P p = cur.second;
					p.real(p.real() + dx*jmp);
					p.imag(p.imag() + dy*jmp);
					pair<double, P> np{func(p), p};
					if (np.first < cur.first ||
					    (np.first == cur.first && PointLess{}(np.second, cur.second))) cur = np, imp = 1;
				}
			}
		}

		if (abs(dist(cur.second - a) - ra) < 1e-9 &&
		    abs(dist(cur.second - b) - rb) < 1e-9) {
			assert(ret);
			assert(dist(out.first - cur.second) < 1e-6 || dist(out.second - cur.second) < 1e-6);
		} else {
			assert(!ret);
		}

		// cerr << '.';
		continue;
skip:;
		// Sometimes hill-climbing is slow, for some reason. :(
		// cerr << '#';
	}
	cout<<"Tests passed!"<<endl;
}
