// SparseLazySegmentTree.h customized to affine composition on [0, N).
const int MOD = 998244353;
struct Node {
	using V = pii;
	using U = int;
	using T = Node;
	static constexpr V id = {1, 0};
	static constexpr V def = {1, 0};
	static constexpr U idU = 0;
	V binop(V L, V R) {
		return {int(1LL * R.first * L.first % MOD),
			int((1LL * R.first * L.second + R.second) % MOD)};
	}
	V applyUpdate(U, V v) { return v; }
	U mergeUpdate(U, U nw) { return nw; }
	T *lt = 0, *rt = 0;
	V val = def;
	U lazy = idU;
	void updateNode(int, int, U u) {
		lazy = mergeUpdate(lazy, u);
		val = applyUpdate(u, val);
	}
	void push(int l, int r) {
		if (!lt) {
			lt = new T();
			rt = new T();
		}
		int mid = l + (r - l) / 2;
		lt->updateNode(l, mid, lazy);
		rt->updateNode(mid + 1, r, lazy);
		lazy = idU;
	}
	void set(int l, int r, int i, V v) {
		if (i < l || r < i) return;
		if (l == r) { val = v; lazy = idU; return; }
		push(l, r);
		int mid = l + (r - l) / 2;
		if (i <= mid) lt->set(l, mid, i, v);
		else rt->set(mid + 1, r, i, v);
		val = binop(lt->val, rt->val);
	}
	V query(int l, int r, int ql, int qr) {
		if (qr < l || r < ql) return id;
		if (ql <= l && r <= qr) return val;
		push(l, r);
		int mid = l + (r - l) / 2;
		return binop(lt->query(l, mid, ql, qr),
			rt->query(mid + 1, r, ql, qr));
	}
};

int main() {
	cin.tie(0)->sync_with_stdio(0);
	int n, q;
	cin >> n >> q;
	Node* root = new Node();
	int L = 0, R = n - 1;
	rep(i, 0, q) {
		int ty;
		cin >> ty;
		if (ty == 0) {
			int p, c, d;
			cin >> p >> c >> d;
			root->set(L, R, p, {c, d});
		} else {
			int l, r, x;
			cin >> l >> r >> x;
			auto [a, b] = root->query(L, R, l, r - 1);
			cout << (1LL * a * x + b) % MOD << '\n';
		}
	}
}
