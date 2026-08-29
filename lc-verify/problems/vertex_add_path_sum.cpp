// LazyUpdateTree + HLD: sum on vertices (header comments: customize op).
struct LazyUpdateTree {
	using V = ll;
	using U = ll;
	static constexpr V id = 0;
	static constexpr V def = 0;
	static constexpr U idU = 0;
	V binop(V a, V b) { return a + b; }
	V applyUpdate(U u, V v) { return v + u; }
	U mergeUpdate(U oldU, U nw) { return oldU + nw; }
	vector<V> arr;
	vector<U> lazy;
	int size;
	LazyUpdateTree(int n) :
		arr(4 * n + 2, def), lazy(4 * n + 2, idU), size(n) {}
	void updateNode(int cur, int, int, U u) {
		lazy[cur] = mergeUpdate(lazy[cur], u);
		arr[cur] = applyUpdate(u, arr[cur]);
	}
	void push(int cur, int l, int r) {
		int mid = l + (r - l) / 2;
		updateNode(2 * cur, l, mid, lazy[cur]);
		updateNode(2 * cur + 1, mid + 1, r, lazy[cur]);
		lazy[cur] = idU;
	}
	void update(int cur, int l, int r, int ql, int qr, U u) {
		if (qr < l || r < ql) return;
		if (ql <= l && r <= qr) {
			updateNode(cur, l, r, u); return;
		}
		push(cur, l, r);
		int mid = l + (r - l) / 2;
		update(2 * cur, l, mid, ql, qr, u);
		update(2 * cur + 1, mid + 1, r, ql, qr, u);
		arr[cur] = binop(arr[2 * cur], arr[2 * cur + 1]);
	}
	void update(int ql, int qr, U u) {
		update(1, 0, size - 1, ql, qr, u);
	}
	void set(int cur, int l, int r, int i, V v) {
		if (i < l || r < i) return;
		if (l == r) { arr[cur] = v; lazy[cur] = idU; return; }
		push(cur, l, r);
		int mid = l + (r - l) / 2;
		if (i <= mid) set(2 * cur, l, mid, i, v);
		else set(2 * cur + 1, mid + 1, r, i, v);
		arr[cur] = binop(arr[2 * cur], arr[2 * cur + 1]);
	}
	void set(int i, V v) { set(1, 0, size - 1, i, v); }
	V query(int cur, int l, int r, int ql, int qr) {
		if (qr < l || r < ql) return id;
		if (ql <= l && r <= qr) return arr[cur];
		push(cur, l, r);
		int mid = l + (r - l) / 2;
		return binop(query(2 * cur, l, mid, ql, qr),
			query(2 * cur + 1, mid + 1, r, ql, qr));
	}
	V query(int ql, int qr) {
		return query(1, 0, size - 1, ql, qr);
	}
};

template <bool VALS_EDGES> struct HLD {
	int N, tim = 0;
	vector<vi> adj;
	vi par, siz, rt, pos;
	LazyUpdateTree tree;
	HLD(vector<vi> adj_)
		: N(sz(adj_)), adj(adj_), par(N, -1), siz(N, 1),
		  rt(N), pos(N), tree(N) { dfsSz(0); dfsHld(0); }
	void dfsSz(int v) {
		for (int& u : adj[v]) {
			adj[u].erase(find(all(adj[u]), v));
			par[u] = v;
			dfsSz(u);
			siz[v] += siz[u];
			if (siz[u] > siz[adj[v][0]]) swap(u, adj[v][0]);
		}
	}
	void dfsHld(int v) {
		pos[v] = tim++;
		for (int u : adj[v]) {
			rt[u] = (u == adj[v][0] ? rt[v] : u);
			dfsHld(u);
		}
	}
	template <class B> void process(int u, int v, B op) {
		for (;; v = par[rt[v]]) {
			if (pos[u] > pos[v]) swap(u, v);
			if (rt[u] == rt[v]) break;
			op(pos[rt[v]], pos[v] + 1);
		}
		op(pos[u] + VALS_EDGES, pos[v] + 1);
	}
	ll queryPath(int u, int v) {
		ll res = tree.id;
		process(u, v, [&](int l, int r) {
			res = tree.binop(res, tree.query(l, r - 1));
		});
		return res;
	}
	ll querySubtree(int v) {
		return tree.query(pos[v] + VALS_EDGES,
			pos[v] + siz[v] - 1);
	}
};

int main() {
	cin.tie(0)->sync_with_stdio(0);
	int n, q;
	cin >> n >> q;
	vector<ll> a(n);
	rep(i, 0, n) cin >> a[i];
	vector<vi> adj(n);
	rep(i, 0, n - 1) {
		int u, v;
		cin >> u >> v;
		adj[u].pb(v);
		adj[v].pb(u);
	}
	HLD<false> hld(adj);
	rep(i, 0, n) hld.tree.set(hld.pos[i], a[i]);
	rep(i, 0, q) {
		int ty;
		cin >> ty;
		if (ty == 0) {
			int p;
			ll x;
			cin >> p >> x;
			hld.tree.update(hld.pos[p], hld.pos[p], x);
		} else {
			int u, v;
			cin >> u >> v;
			cout << hld.queryPath(u, v) << '\n';
		}
	}
}
