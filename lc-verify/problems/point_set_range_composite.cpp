// LazyUpdateTree customized to affine composition (point set, range query).
const int MOD = 998244353;
struct LazyUpdateTree {
	using V = pii; // (a, b) : x -> a x + b
	using U = int;
	static constexpr V id = {1, 0};
	static constexpr V def = {1, 0};
	static constexpr U idU = 0;
	V binop(V L, V R) { // apply L then R
		return {int(1LL * R.first * L.first % MOD),
			int((1LL * R.first * L.second + R.second) % MOD)};
	}
	V applyUpdate(U, V v) { return v; }
	U mergeUpdate(U, U nw) { return nw; }
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

int main() {
	cin.tie(0)->sync_with_stdio(0);
	int n, q;
	cin >> n >> q;
	LazyUpdateTree st(n);
	rep(i, 0, n) {
		int a, b;
		cin >> a >> b;
		st.set(i, {a, b});
	}
	rep(i, 0, q) {
		int ty;
		cin >> ty;
		if (ty == 0) {
			int p, c, d;
			cin >> p >> c >> d;
			st.set(p, {c, d});
		} else {
			int l, r, x;
			cin >> l >> r >> x;
			auto [a, b] = st.query(l, r - 1);
			cout << (1LL * a * x + b) % MOD << '\n';
		}
	}
}
