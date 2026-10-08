/**
 * Author: Kimiyuki Onaka (kmyk), adapted
 * Date: 2026-10-08
 * License: MIT
 * Source: kmyk/competitive-programming-library
 *  graph/yen_algorithm.hpp
 * Description: Up to $k$ shortest simple paths from $s$ to
 *  $t$, sorted by cost; ties arbitrary. Returns (cost,
 *  vertex sequence), including both endpoints. $g[u]$
 *  contains (to, weight); $n=|g|$, vertices in $[0,n)$. No
 *  parallel edges. Weights and all simple path costs
 *  must be in $[0,2^{60})$. For undirected graphs add
 *  both directions. $k=0$ or unreachable returns empty;
 *  $s=t$ returns only $(0,\{s\})$ when $k>0$.
 * Time: $O(kn((m+n+k)\log n+n\log k))$
 * Memory: $O(m+kn)$
 * Status: stress-tested
 */
#pragma once

vector<pair<ll, vi>> yen(
	const vector<vector<pair<int, ll>>>& g,
	int s, int t, int k) {
	if (!k) return {};
	int n = sz(g);
	const ll inf = 1LL << 60;
	using Path = pair<ll, vi>;
	using pli = pair<ll, int>;
	vi ban(n);
	set<pii> cut;
	map<pii, ll> w;
	rep(v,0,n) for (auto [u, c] : g[v]) w[{v,u}] = c;
	auto dij = [&](int start) -> Path {
		vector<ll> d(n, inf);
		vi pre(n, -1), p;
		priority_queue<pli, vector<pli>, greater<pli>> q;
		d[start] = 0; q.push({0,start});
		while (!q.empty()) {
			auto [dv, v] = q.top(); q.pop();
			if (dv != d[v]) continue;
			if (v == t) break;
			for (auto [u, c] : g[v])
				if (!ban[u] && !cut.count({v,u}) && dv+c<d[u]) {
					d[u] = dv+c; pre[u] = v; q.push({d[u],u});
				}
		}
		if (d[t] == inf) return {inf,{}};
		for (int v = t; v != -1; v = pre[v]) p.pb(v);
		reverse(all(p));
		return {d[t],p};
	};
	Path first = dij(s);
	if (first.sc.empty()) return {};
	vector<Path> ans{first};
	set<Path> cand;
	while (sz(ans) < k) {
		vi p = ans.back().sc, match(sz(ans));
		iota(all(match), 0);
		fill(all(ban), 0);
		ll cost = 0;
		rep(i,0,sz(p)-1) {
			cut.clear();
			vi next;
			for (int j : match) {
				vi& a = ans[j].sc;
				if (sz(a)>i+1 && a[i]==p[i]) {
					cut.insert({p[i],a[i+1]}); next.pb(j);
				}
			}
			match = next; // paths sharing this prefix
			auto [d, tail] = dij(p[i]);
			if (!tail.empty()) {
				vi path(p.begin(), p.begin()+i);
				path.insert(path.end(), all(tail));
				cand.insert({cost+d,path});
				if (sz(cand) > k-sz(ans))
					cand.erase(prev(cand.end()));
			}
			ban[p[i]] = 1; cost += w[{p[i],p[i+1]}];
		}
		if (cand.empty()) break;
		ans.pb(*cand.begin()); cand.erase(cand.begin());
	}
	return ans;
}

/// MIT License
/// Copyright (c) 2014-2019 Kimiyuki Onaka
///
/// Permission is hereby granted, free of charge, to any person
/// obtaining a copy of this software and associated
/// documentation files (the "Software"), to deal in the
/// Software without restriction, including without limitation
/// the rights to use, copy, modify, merge, publish,
/// distribute, sublicense, and/or sell copies of the
/// Software, and to
/// permit persons to whom the Software is furnished to do so,
/// subject to the following conditions:
///
/// The above copyright notice and this permission notice shall
/// be included in all copies or substantial portions of the
/// Software.
///
/// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY
/// KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
/// WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
/// PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS
/// OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR
/// OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
/// OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
/// SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
