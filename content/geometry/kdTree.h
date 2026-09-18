/**
 * Author: Stanford
 * Date: Unknown
 * Source: Stanford Notebook
 * Description: KD-tree (2d, can be extended to 3d)
 * Status: Tested on excellentengineers
 */
#pragma once

#include "Point.h"

typedef long long T;
typedef complex<T> P;
const T INF = numeric_limits<T>::max();

bool on_x(P a, P b) { return a.real() < b.real(); }
bool on_y(P a, P b) { return a.imag() < b.imag(); }

struct Node {
	P pt; // if this is a leaf, the single point in it
	T x0 = INF, x1 = -INF, y0 = INF, y1 = -INF; // bounds
	Node *first = 0, *second = 0;

	T distance(P p) { // min squared distance to a point
		T x = (p.real() < x0 ? x0 : p.real() > x1 ? x1 : p.real());
		T y = (p.imag() < y0 ? y0 : p.imag() > y1 ? y1 : p.imag());
		return norm(P(x,y) - p);
	}

	Node(vector<P>&& vp) : pt(vp[0]) {
		for (P p : vp) {
			x0 = min(x0, p.real()); x1 = max(x1, p.real());
			y0 = min(y0, p.imag()); y1 = max(y1, p.imag());
		}
		if (vp.size() > 1) {
			// split on x if width >= height (not ideal...)
			sort(all(vp), x1 - x0 >= y1 - y0 ? on_x : on_y);
			// divide by taking half the array for each child (not
			// best performance with many duplicates in the middle)
			int half = sz(vp)/2;
			first = new Node({vp.begin(), vp.begin() + half});
			second = new Node({vp.begin() + half, vp.end()});
		}
	}
};

struct KDTree {
	Node* root;
	KDTree(const vector<P>& vp) : root(new Node({all(vp)})) {}

	pair<T, P> search(Node *node, P p) {
		if (!node->first) {
			// uncomment if we should not find the point itself:
			// if (p == node->pt) return {INF, P()};
			return make_pair(norm(p - node->pt), node->pt);
		}

		Node *f = node->first, *s = node->second;
		T bfirst = f->distance(p), bsec = s->distance(p);
		if (bfirst > bsec) swap(bsec, bfirst), swap(f, s);

		// search closest side first, other side if needed
		auto best = search(f, p);
		if (bsec < best.first) {
			auto other = search(s, p);
			if (other.first < best.first) best = other;
		}
		return best;
	}

	// find nearest point to a point, and its squared distance
	// Requires a nonempty point set.
	pair<T, P> nearest(P p) {
		return search(root, p);
	}
};
