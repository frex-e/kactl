/**
 * Author: Ulf Lundstrom
 * Date: 2009-04-07
 * License: CC0
 * Source: My geometric reasoning
 * Description: Returns the shortest distance on the sphere with radius \texttt{radius} between the points
 * with azimuthal angles (longitude) \texttt{f1} ($\phi_1$) and \texttt{f2} ($\phi_2$) from x axis and zenith angles
 * (latitude) \texttt{t1} ($\theta_1$) and \texttt{t2} ($\theta_2$) from z axis (0 = north pole). All angles measured
 * in radians. Converts to Cartesian unit vectors and uses
 * $2\operatorname{atan2}(|b-a|,|b+a|)$, stable also near
 * coincident and antipodal points.
 * Status: stress-tested, tested on kattis:airlinehub
 */
#pragma once

double sphericalDistance(double f1, double t1,
		double f2, double t2, double radius) {
	array<double,3> a = {sin(t1)*cos(f1),
		sin(t1)*sin(f1), cos(t1)};
	array<double,3> b = {sin(t2)*cos(f2),
		sin(t2)*sin(f2), cos(t2)};
	double d = 0, s = 0;
	rep(i,0,3) {
		d += (b[i]-a[i])*(b[i]-a[i]);
		s += (b[i]+a[i])*(b[i]+a[i]);
	}
	return radius*2*atan2(sqrt(d), sqrt(s));
}
