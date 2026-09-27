/**
 * Author: chilli, Takanori MAEHARA
 * Date: 2019-11-02
 * License: CC0
 * Source: https://github.com/spaghetti-source/algorithm/blob/master/geometry/rectilinear_mst.cc
 * Description: Given N points, returns up to 4*N edges, which are guaranteed
 * to contain a minimum spanning tree for the graph with edge weights $w(p, q) =$
 * \texttt{|p.real() - q.real()| + |p.imag() - q.imag()|}.
 * Edges are \texttt{MEdge\{distance, src, dst\}}. Use a
 * standard MST algorithm on the result to find the final MST.
 * Time: O(N \log N)
 * Status: Stress-tested
 */
#pragma once
#include "Point.h"

struct MEdge { double d; int a, b; };
vector<MEdge> manhattanMST(vector<pp> ps) {
	vi id(sz(ps));
	iota(all(id), 0);
	vector<MEdge> edges;
	rep(k,0,4) {
		sort(all(id), [&](int i, int j) {
		     return (ps[i]-ps[j]).real() < (ps[j]-ps[i]).imag();});
		map<double, int> sweep;
		for (int i : id) {
			for (auto it = sweep.lower_bound(-ps[i].imag());
				        it != sweep.end(); sweep.erase(it++)) {
				int j = it->second;
				pp d = ps[i] - ps[j];
				if (d.imag() > d.real()) break;
				edges.push_back({d.imag() + d.real(), i, j});
			}
			sweep[-ps[i].imag()] = i;
		}
		for (pp& p : ps)
			p = k & 1 ? pp(-p.real(), p.imag())
			            : pp(p.imag(), p.real());
	}
	return edges;
}
