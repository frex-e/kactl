/**
 * Author: Benq
 * Date: 2026-08-21
 * License: CC0
 * Source: https://github.com/caterpillow/cactl
 *  (originally https://github.com/bqi343/USACO)
 * Description: Dominator tree of a directed graph.
 *  Assumes every node is reachable from $root$. $a$
 *  dominates $b$ iff every path from $root$ to $b$
 *  passes through $a$. Construct with the adjacency
 *  list and root; then \texttt{ans[u]} holds the
 *  children of $u$ in the dominator tree.
 * Time: $O(M\log N)$
 * Status: stress-tested
 */
#pragma once

struct Dominator {
	int n, co = 0;
	vector<vi> adj, ans, radj, child, sdomChild;
	vi label, rlabel, sdom, dom, par, bes;
	int get(int x) { // DSU with path compression
		// vertex with smallest sdom on path to root
		if (par[x] != x) {
			int t = get(par[x]); par[x] = par[par[x]];
			if (sdom[t] < sdom[bes[x]]) bes[x] = t;
		}
		return bes[x];
	}
	void dfs(int x) { // create DFS tree; labels 0..n-1
		label[x] = co; rlabel[co] = x;
		sdom[co] = par[co] = bes[co] = co;
		++co;
		for (int y : adj[x]) {
			if (label[y] < 0) {
				dfs(y); child[label[x]].pb(label[y]); }
			radj[label[y]].pb(label[x]);
		}
	}
	Dominator(const vector<vi>& g, int root) : n(sz(g)),
		adj(g), ans(n), radj(n), child(n), sdomChild(n),
		label(n, -1), rlabel(n), sdom(n),
		dom(n), par(n), bes(n) {
		dfs(root);
		for (int i = co - 1; i >= 0; --i) {
			for (int j : radj[i])
				sdom[i] = min(sdom[i], sdom[get(j)]);
			if (i) sdomChild[sdom[i]].pb(i);
			for (int j : sdomChild[i]) {
				int k = get(j);
				if (sdom[j] == sdom[k]) dom[j] = sdom[j];
				else dom[j] = k;
			}
			for (int j : child[i]) par[j] = i;
		}
		rep(i,1,co) {
			if (dom[i] != sdom[i]) dom[i] = dom[dom[i]];
			ans[rlabel[dom[i]]].pb(rlabel[i]);
		}
	}
};
