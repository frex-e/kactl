/**
 * Author: ei1333 (adapted)
 * Date: 2026-08-20
 * License: CC0
 * Source: https://github.com/ei1333/library GabowEdmonds
 *  (Library Checker general\_matching; qiita.com/Kutimoti\_T)
 * Description: Maximum cardinality matching in general graphs
 *  (Gabow--Edmonds).
 *  0-indexed. \texttt{ae} adds an undirected edge.
 *  After \texttt{solve()}, \texttt{mate[u]} is the partner
 *  of $u$, or $-1$ if unmatched. Returns matching size.
 * Time: $O(NM)$
 * Status: stress-tested
 */
#pragma once

struct Blossom {
	struct Edge { int to, idx; };
	int n;
	vector<vector<Edge>> g;
	vector<pii> edges;
	vi mt, label, first, q, mate;
	Blossom(int n) : n(n), g(n + 1), mt(n + 1),
		label(n + 1, -1), first(n + 1) {}
	void ae(int u, int v) {
		if (u == v) return;
		++u, ++v;
		g[u].push_back({v, sz(edges) + n + 1});
		g[v].push_back({u, sz(edges) + n + 1});
		edges.emplace_back(u, v);
	}
	int find(int x) {
		return label[first[x]] < 0 ? first[x]
			: first[x] = find(first[x]);
	}
	void rematch(int v, int w) {
		int t = mt[v];
		mt[v] = w;
		if (mt[t] != v) return;
		if (label[v] <= n) {
			mt[t] = label[v];
			rematch(label[v], t);
		} else {
			auto [x, y] = edges[label[v] - n - 1];
			rematch(x, y);
			rematch(y, x);
		}
	}
	void assign_label(int x, int y, int num) {
		int r = find(x), s = find(y), join = 0;
		if (r == s) return;
		label[r] = label[s] = -num;
		while (1) {
			if (s) swap(r, s);
			r = find(label[mt[r]]);
			if (label[r] == -num) { join = r; break; }
			label[r] = -num;
		}
		for (int v : {first[x], first[y]}) {
			while (v != join) {
				q.push_back(v);
				label[v] = num;
				first[v] = join;
				v = first[label[mt[v]]];
			}
		}
	}
	bool augment(int u) {
		q = {u};
		first[u] = 0;
		label[u] = 0;
		for (int qi = 0; qi < sz(q); ++qi) {
			int x = q[qi];
			for (auto e : g[x]) {
				int y = e.to;
				if (!mt[y] && y != u) {
					mt[y] = x;
					rematch(x, y);
					return 1;
				} else if (label[y] >= 0)
					assign_label(x, y, e.idx);
				else if (label[mt[y]] < 0) {
					label[mt[y]] = x;
					first[mt[y]] = y;
					q.push_back(mt[y]);
				}
			}
		}
		return 0;
	}
	int solve() {
		rep(i,1,n+1) if (!mt[i] && augment(i))
			label.assign(n + 1, -1);
		int ans = 0;
		mate.assign(n, -1);
		rep(i,1,n+1) if (mt[i]) {
			mate[i - 1] = mt[i] - 1;
			if (i < mt[i]) ++ans;
		}
		return ans;
	}
};
