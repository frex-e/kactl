#include "../utilities/template.h"
#include "../../content/geometry/sphericalDistance.h"

long double reference(double f1, double t1,
		double f2, double t2) {
	array<long double,3> a = {sinl(t1)*cosl(f1),
		sinl(t1)*sinl(f1),cosl(t1)};
	array<long double,3> b = {sinl(t2)*cosl(f2),
		sinl(t2)*sinl(f2),cosl(t2)};
	long double dot = 0, cross = 0;
	rep(i,0,3) {
		dot += a[i]*b[i];
		int j = (i+1)%3, k = (i+2)%3;
		long double c = a[j]*b[k]-a[k]*b[j];
		cross += c*c;
	}
	return atan2l(sqrtl(cross),dot);
}

int main() {
	double pi = acos(-1.);
	auto check = [&](double f1, double t1, double f2,
			double t2, double radius) {
		double got = sphericalDistance(f1,t1,f2,t2,radius);
		double expected = double(reference(f1,t1,f2,t2))*radius;
		assert(isfinite(got));
		assert(got >= 0 && got <= pi*radius+1e-14*radius);
		assert(abs(got-expected) <= 3e-15*radius);
		assert(abs(got-sphericalDistance(f2,t2,f1,t1,radius))
			<= 1e-15*radius);
	};
	assert(sphericalDistance(0,0,0,0,1) == 0);
	check(0,0,0,pi,1);
	check(0,pi/2,pi/2,pi/2,6371);
	// This antipodal pair rounded the old asin argument above 1.
	check(-0.78376514985028811,1.6923623444979192,
		-0.78376514985028811+pi,pi-1.6923623444979192,1);
	for (double delta : {0.,1e-15,1e-12,1e-9,1e-6}) {
		check(0,pi/2,pi-delta,pi/2,1);
		check(0,pi/2,delta,pi/2,1);
		assert(abs(sphericalDistance(0,pi/2,pi-delta,pi/2,1)
			-(pi-delta)) < 1e-15);
	}
	mt19937 rng(42);
	uniform_real_distribution<double> lon(-pi,pi), zen(0,pi);
	rep(it,0,100000) {
		double f = lon(rng), t = zen(rng);
		check(f,t,lon(rng),zen(rng),6371);
		check(f,t,f+pi,pi-t,1);
		check(f,t,f+pi-1e-9,pi-t,1);
		check(f,t,f,t,1);
	}
	cout << "Tests passed!" << endl;
}
