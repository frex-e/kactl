/**
 * Author: Ulf Lundstrom with inspiration from tinyKACTL
 * Date: 2009-04-14
 * License: CC0
 * Source:
 * Description: Handles points in 3D space using doubles.
 *  Angle ranges: phi in $[-\pi,\pi]$, theta in $[0,\pi]$;
 *  both endpoints included in each range.
 * Usage:
 * Status: tested, except for phi and theta
 */
#pragma once

struct Point3D {
	double x, y, z;
	explicit Point3D(double x=0, double y=0, double z=0) :
		x(x), y(y), z(z) {}
	bool operator<(Point3D p) const {
		return tie(x, y, z) < tie(p.x, p.y, p.z); }
	bool operator==(Point3D p) const {
		return tie(x, y, z) == tie(p.x, p.y, p.z); }
	Point3D operator+(Point3D p) const {
		return Point3D(x+p.x, y+p.y, z+p.z); }
	Point3D operator-(Point3D p) const {
		return Point3D(x-p.x, y-p.y, z-p.z); }
	Point3D operator*(double d) const {
		return Point3D(x*d, y*d, z*d); }
	Point3D operator/(double d) const {
		return Point3D(x/d, y/d, z/d); }
	double dot(Point3D p) const { return x*p.x + y*p.y + z*p.z; }
	Point3D cross(Point3D p) const {
		return Point3D(y*p.z-z*p.y, z*p.x-x*p.z, x*p.y-y*p.x);
	}
	double dist2() const { return x*x + y*y + z*z; }
	double dist() const { return sqrt(dist2()); }
	//Azimuthal angle (longitude) to x-axis in interval [-pi, pi]
	double phi() const { return atan2(y, x); } 
	//Zenith angle (latitude) to the z-axis in interval [0, pi]
	double theta() const { return atan2(sqrt(x*x+y*y),z); }
	Point3D unit() const { return *this/dist(); } //makes dist()=1
	//returns unit vector normal to *this and p
	Point3D normal(Point3D p) const { return cross(p).unit(); }
	//returns point rotated 'angle' radians ccw around axis
	Point3D rotate(double angle, Point3D axis) const {
		double s = sin(angle), c = cos(angle);
		Point3D u = axis.unit();
		return u*dot(u)*(1-c) + (*this)*c - cross(u)*s;
	}
};
