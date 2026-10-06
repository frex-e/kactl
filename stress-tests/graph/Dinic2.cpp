#include "../utilities/template.h"
#include "../../content/graph/Dinic.h"
#include "../../content/graph/Dinic2.h"

// Capacity edits follow cactl's Dinic2 test, with self-loops,
// conservation checks, terminal-edge cancellation, and large caps.
struct Rec { int a, i, b; ll c, rc; };

ll brute(const vector<Rec>& es, int n, int s, int t) {
	ll best = LLONG_MAX;
	rep(mask,0,1 << n) {
		if (!(mask >> s & 1) || (mask >> t & 1)) continue;
		ll cut = 0;
		for (auto e : es) {
			if ((mask >> e.a & 1) && !(mask >> e.b & 1))
				cut += e.c;
			if ((mask >> e.b & 1) && !(mask >> e.a & 1))
				cut += e.rc;
		}
		best = min(best, cut);
	}
	return best;
}

void check(Dinic2& d, const vector<Rec>& es,
		int s, int t, ll flow, bool maximum) {
	vector<ll> balance(sz(d.adj));
	ll cut = 0;
	for (auto r : es) {
		auto& e = d.adj[r.a][r.i];
		assert(e.to == r.b && e.rev >= 0);
		assert(e.rev < sz(d.adj[r.b]));
		auto& re = d.adj[r.b][e.rev];
		assert(re.to == r.a && re.rev == r.i);
		if (r.a == r.b) assert(e.rev != r.i);
		assert(e.c >= 0 && re.c >= 0);
		assert(e.c + re.c == r.c + r.rc);
		ll f = r.c - e.c;
		assert(f == re.c - r.rc && -r.rc <= f && f <= r.c);
		if (r.a == r.b) assert(f == 0);
		balance[r.a] += f;
		balance[r.b] -= f;
		if (maximum) {
			if (d.leftOfMinCut(r.a) && !d.leftOfMinCut(r.b)) {
				assert(e.c == 0);
				cut += r.c;
			}
			if (d.leftOfMinCut(r.b) && !d.leftOfMinCut(r.a)) {
				assert(re.c == 0);
				cut += r.rc;
			}
		}
	}
	rep(v,0,sz(balance))
		assert(balance[v] == (v == s ? flow : v == t ? -flow : 0));
	if (maximum) {
		assert(d.leftOfMinCut(s) && !d.leftOfMinCut(t));
		assert(cut == flow);
	}
}

int main() {
	{ // Exact limit needs one more call to certify the cut.
		Dinic2 d(3);
		d.addEdge(0, 0, 7, 2);
		d.addEdge(0, 1, 10);
		d.addEdge(1, 2, 10);
		assert(d.adj[0][0].rev == 1 && d.adj[0][1].rev == 0);
		assert(d.calc(0, 2, 0) == 0);
		assert(d.calc(0, 2, 3) == 3);
		assert(d.calc(0, 2, 7) == 7);
		assert(d.calc(0, 2) == 0);
		assert(d.leftOfMinCut(0) && !d.leftOfMinCut(2));
		d = Dinic2(3);
		assert(d.adj[0].empty() && d.calc(0, 2) == 0);
	}
	{ // DFS depth and 64-bit capacities, with loops on the path.
		int n = 2000;
		Dinic2 d(n);
		ll c = 1LL << 45;
		rep(i,0,n) {
			d.addEdge(i, i, c, c);
			if (i + 1 < n) d.addEdge(i, i + 1, c);
		}
		assert(d.calc(0, n - 1, c - 1) == c - 1);
		assert(d.calc(0, n - 1) == 1);
		assert(!d.leftOfMinCut(n - 1));
	}

	mt19937 rng(7);
	rep(it,0,30000) {
		int n = 2 + int(rng() % 7), s = int(rng() % n);
		int t = int(rng() % (n - 1));
		if (t >= s) t++;
		Dinic2 d(n);
		vector<Rec> es;
		ll scale = it % 3 == 0 ? 1LL << 40 : 1;
		auto add = [&]() {
			int a = int(rng() % n), b = int(rng() % n);
			ll c = (rng() % 11) * scale;
			ll rc = rng() % 4 == 0 ? (rng() % 11) * scale : 0;
			es.push_back({a, sz(d.adj[a]), b, c, rc});
			d.addEdge(a, b, c, rc);
		};
		int m = int(rng() % 30);
		rep(e,0,m) add();
		ll best = brute(es, n, s, t);
		Dinic old(n);
		for (auto r : es) old.addEdge(r.a, r.b, r.c, r.rc);
		assert(old.calc(s, t) == best);
		Dinic2 full = d;
		assert(full.calc(s, t) == best);
		check(full, es, s, t, best, true);

		ll lim = it % 2 ? rng() % 30 : best;
		ll flow = d.calc(s, t, lim);
		assert(flow == min(lim, best));
		check(d, es, s, t, flow, false);
		flow += d.calc(s, t);
		assert(flow == best && d.calc(s, t) == 0);
		check(d, es, s, t, flow, true);

		rep(round,0,3) {
			if (es.empty() || rng() % 3 == 0) {
				add(); // Adding edges to an existing flow is legal.
			} else {
				auto& r = es[rng() % es.size()];
				auto& e = d.adj[r.a][r.i];
				auto& re = d.adj[r.b][e.rev];
				if (rng() % 2) {
					ll delta = (rng() % 11) * scale;
					e.c += delta, r.c += delta;
				} else {
					ll f = r.c - e.c, nc = rng() % (r.c + 1);
					if (f <= nc) e.c -= r.c - nc;
					else {
						ll x = f - nc;
						e.c = 0, re.c -= x;
						ll z = x - d.calc(r.a, r.b, x);
						if (r.a != s) assert(d.calc(r.a, s, z) == z);
						if (r.b != t) assert(d.calc(t, r.b, z) == z);
						flow -= z;
					}
					r.c = nc;
				}
			}
			check(d, es, s, t, flow, false);
			flow += d.calc(s, t);
			assert(flow == brute(es, n, s, t));
			assert(d.calc(s, t) == 0);
			check(d, es, s, t, flow, true);
		}
	}
	cout << "Tests passed!" << endl;
}
