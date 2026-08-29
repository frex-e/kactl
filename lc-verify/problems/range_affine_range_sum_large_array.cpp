// SparseLazySegmentTree.h customized to affine range update / range sum.
const int MOD = 998244353;
struct Node {
	using V = ll;
	using U = pii; // (b, c): x -> b x + c
	using T = Node;
	static constexpr V id = 0;
	static constexpr V def = 0;
	static constexpr U idU = {1, 0};
	V binop(V a, V b) { return a + b >= MOD ? a + b - MOD : a + b; }
	V applyUpdate(U u, V v) {
		return (u.first * v + (ll)u.second * seglen) % MOD;
	}
	U mergeUpdate(U o, U nw) {
		return {int((ll)nw.first * o.first % MOD),
			int(((ll)nw.first * o.second + nw.second) % MOD)};
	}
	T *lt = 0, *rt = 0;
	V val = def;
	U lazy = idU;
	int seglen = 1;
	void updateNode(int l, int r, U u) {
		seglen = r - l + 1;
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
	void update(int l, int r, int ql, int qr, U u) {
		if (qr < l || r < ql) return;
		if (ql <= l && r <= qr) {
			updateNode(l, r, u);
			return;
		}
		push(l, r);
		int mid = l + (r - l) / 2;
		lt->update(l, mid, ql, qr, u);
		rt->update(mid + 1, r, ql, qr, u);
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
	~Node() { delete lt; delete rt; }
};

int main() {
	cin.tie(0)->sync_with_stdio(0);
	int n, q;
	cin >> n >> q;
	Node* root = new Node();
	int L = 0, R = n - 1;
	rep(i, 0, q) {
		int ty, l, r;
		cin >> ty >> l >> r;
		if (ty == 0) {
			int b, c;
			cin >> b >> c;
			root->update(L, R, l, r - 1, {b, c});
		} else {
			cout << root->query(L, R, l, r - 1) << '\n';
		}
	}
}
