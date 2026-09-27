#include "../utilities/template.h"

#include "../../content/geometry/Point.h"

long long distanceChecks = 0;
double countedNorm(pp p) {
	++distanceChecks;
	return norm(p);
}
#define norm countedNorm
#include "../../content/geometry/ClosestPair.h"
#undef norm

namespace old {
template<class It>
bool it_less(const It& i, const It& j) { return PointLess{}(*i, *j); }
template<class It>
bool y_it_less(const It& i,const It& j) {return i->imag() < j->imag();}

template<class It, class IIt> /* IIt = vector<It>::iterator */
double cp_sub(IIt ya, IIt yaend, IIt xa, It &i1, It &i2) {
	typedef typename iterator_traits<It>::value_type P;
	int n = yaend-ya, split = n/2;
	if(n <= 3) { // base case
		double a = dist(*xa[1]-*xa[0]), b = 1e50, c = 1e50;
		if(n==3) b=dist(*xa[2]-*xa[0]), c=dist(*xa[2]-*xa[1]);
		if(a <= b) { i1 = xa[1];
			if(a <= c) return i2 = xa[0], a;
			else return i2 = xa[2], c;
		} else { i1 = xa[2];
			if(b <= c) return i2 = xa[0], b;
			else return i2 = xa[1], c;
	}	}
	vector<It> ly, ry, stripy;
	P splitp = *xa[split];
	double splitx = splitp.real();
	for(IIt i = ya; i != yaend; ++i) { // Divide
		if(*i != xa[split] && norm(**i-splitp) < 1e-12)
			return i1 = *i, i2 = xa[split], 0;// nasty special case!
		if (PointLess{}(**i, splitp)) ly.push_back(*i);
		else ry.push_back(*i);
	} // assert((signed)lefty.size() == split)
	It j1, j2; // Conquer
	double a = cp_sub(ly.begin(), ly.end(), xa, i1, i2);
	double b = cp_sub(ry.begin(), ry.end(), xa+split, j1, j2);
	if(b < a) a = b, i1 = j1, i2 = j2;
	double a2 = a*a;
	for(IIt i = ya; i != yaend; ++i) { // Create strip (y-sorted)
		double x = (*i)->real();
		if(x >= splitx-a && x <= splitx+a) stripy.push_back(*i);
	}
	for(IIt i = stripy.begin(); i != stripy.end(); ++i) {
		const P &p1 = **i;
		for(IIt j = i+1; j != stripy.end(); ++j) {
			const P &p2 = **j;
			if(p2.imag()-p1.imag() > a) break;
			double d2 = norm(p2-p1);
			if(d2 < a2) i1 = *i, i2 = *j, a2 = d2;
	}	}
	return sqrt(a2);
}

template<class It> // It is random access iterators of point<T>
double closestpair(It begin, It end, It &i1, It &i2 ) {
	vector<It> xa, ya;
	assert(end-begin >= 2);
	for (It i = begin; i != end; ++i)
		xa.push_back(i), ya.push_back(i);
	sort(xa.begin(), xa.end(), it_less<It>);
	sort(ya.begin(), ya.end(), y_it_less<It>);
	return cp_sub(ya.begin(), ya.end(), xa.begin(), i1, i2);
}
}

int main() {
	// Cancellation regression, plus translated/axis-swapped strips.
	auto check = [](const vector<pp>& ps) {
		double best = INFINITY;
		rep(i,0,sz(ps)) rep(j,i+1,sz(ps))
			best = min(best, norm(ps[i] - ps[j]));
		auto [a, b] = closest(ps);
		assert(norm(a - b) == best);
		assert(find(all(ps), a) != ps.end());
		assert(find(all(ps), b) != ps.end());
		if (a == b) assert(count(all(ps), a) >= 2);
	};
	check({{0,1}, {1e-17,1}, {2e-17,1}});
	for (double scale : {1e-100, 1e-17, 1.0, 1e100}) {
		for (double offset : {-1e100, -1.0, 0.0, 1.0, 1e100}) {
			for (int swapAxes : {0, 1}) rep(it,0,1000) {
				vector<pp> ps;
				int n = rand() % 30 + 2;
				rep(i,0,n) {
					double x = scale * (double(rand()) / RAND_MAX);
					double y = offset + scale * (rand() % 3);
					ps.emplace_back(swapAxes ? y : x, swapAxes ? x : y);
				}
				check(ps);
			}
		}
	}
	// Dense fractional inputs must not cause quadratic distance checks.
	for (double scale : {1.0, 1e-9, 1e-100}) {
		int n = 4000;
		vector<pp> ps;
		double best = INFINITY;
		rep(i,0,n) {
			double x = scale * i / n;
			ps.emplace_back(x, x);
			if (i) best = min(best, norm(ps[i] - ps[i-1]));
		}
		distanceChecks = 0;
		auto pa = closest(ps);
		assert(norm(pa.first - pa.second) == best);
		assert(distanceChecks < 10 * n);
	}
	{
		vector<pp> ps(4000, pp(0.125, -0.25));
		distanceChecks = 0;
		auto pa = closest(ps);
		assert(pa.first == ps[0] && pa.second == ps[0]);
		assert(distanceChecks < 10 * sz(ps));
	}
	// Fractional coordinates at different scales, checked by brute force.
	for (double scale : {1.0, 1e-9, 1e-100}) rep(it,0,10000) {
		int n = rand() % 15 + 2;
		vector<pp> ps;
		rep(i,0,n) ps.emplace_back(
			scale * (double(rand()) / RAND_MAX - 0.5),
			scale * (double(rand()) / RAND_MAX - 0.5));
		double best = INFINITY;
		rep(i,0,n) rep(j,i+1,n)
			best = min(best, norm(ps[i] - ps[j]));
		auto pa = closest(ps);
		assert(norm(pa.first - pa.second) == best);
	}
	// Compare against the old code
	double sum = 0;
	int mode = 1;
	if (mode != 0) rep(it,0,100) {
		// clog << it << ' ';
		int n = 100000;
		int maxx = rand() % 1000000 + 1;
		int maxy = rand() % 1000000 + 1;
		int biasx = -100;
		int biasy = -100;
		vector<pp> ps;
		rep(i,0,n) {
			int x = rand() % maxx + biasx;
			int y = rand() % maxy + biasy;
			ps.emplace_back(x, y);
		}
		double foundDist = -1, oldDist = -1, theDist = -1;
		if (mode == 1 || mode == 3) {
			auto pa = closest(ps);
			theDist = foundDist = norm(pa.first - pa.second);
		}
		if (mode == 2 || mode == 3) {
			vector<pp>::iterator i1, i2;
			old::closestpair(all(ps), i1, i2);
			theDist = oldDist = norm(*i1 - *i2);
		}
		sum += theDist;
		// cerr << theDist << endl;
		if (mode == 3 && oldDist != foundDist) {
			cerr << "failed at " << it << endl;
			return 1;
		}
	}
	// cout << sum << endl;

	// Compare against bruteforce
	rep(it,0,1'000'000) {
		int n = rand() % 15 + 2;
		int maxx = rand() % 20 + 1;
		int maxy = rand() % 20 + 1;
		int biasx = rand() % 20 - 10;
		int biasy = rand() % 20 - 10;
		vector<pp> ps;
		rep(i,0,n) {
			int x = rand() % maxx + biasx;
			int y = rand() % maxy + biasy;
			ps.emplace_back(x, y);
		}
		double minDist = INFINITY;
		rep(i,0,n) rep(j,i+1,n) {
			minDist = min(minDist, norm(ps[i] - ps[j]));
		}
		auto pa = closest(ps);
		double foundDist = norm(pa.first - pa.second);
		if (minDist != foundDist) {
			cerr << "failed at " << it << endl;
			return 1;
		}
	}
	cout<<"Tests passed!"<<endl;
}
