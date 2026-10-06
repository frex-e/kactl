/**
 * Author: me
 * Date: 2026-10-06
 * License: CC0
 * Source: me
 * Description: Online sparse 2D segment tree on
 *  $[0,n) \times [0,m)$. Point assignment with
 *  \texttt{set(x,y,v)}, rectangle query with
 *  \texttt{query(x1,x2,y1,y2)} (half-open bounds).
 *  Unassigned points have value \texttt{unit}.
 *  Change \texttt{V}, \texttt{unit}, \texttt{f} for any
 *  commutative monoid. Queries allocate no nodes.
 * Time: $O(\log n \log m)$ per operation.
 * Memory: $O(k \log n \log m)$ for $k$ distinct updated
 *  points. Allocated nodes are retained until destruction.
 * Status: stress-tested
 */
#pragma once

struct Tree2D {
	using V = ll;
	static constexpr V unit = LLONG_MIN;
	static V f(V a, V b) { return max(a, b); }
	struct Y {
		using T = Y;
		T *lt = 0, *rt = 0;
		V val = unit;
		static V get(T* p) { return p ? p->val : unit; }
		void set(int l, int r, int y, V v) {
			if (r - l == 1) { val = v; return; }
			int mid = l + (r - l) / 2;
			T*& ch = y < mid ? lt : rt;
			if (!ch) ch = new T;
			if (y < mid) ch->set(l, mid, y, v);
			else ch->set(mid, r, y, v);
			val = f(get(lt), get(rt));
		}
		V query(int l, int r, int a, int b) const {
			if (b <= l || r <= a) return unit;
			if (a <= l && r <= b) return val;
			int mid = l + (r - l) / 2;
			return f(lt ? lt->query(l, mid, a, b) : unit,
				rt ? rt->query(mid, r, a, b) : unit);
		}
		~Y() { delete lt; delete rt; }
	};
	struct Node {
		using T = Node;
		T *lt = 0, *rt = 0;
		Y tree;
		void set(int l, int r, int m, int x, int y, V v) {
			if (r - l > 1) {
				int mid = l + (r - l) / 2;
				T*& ch = x < mid ? lt : rt;
				if (!ch) ch = new T;
				if (x < mid) ch->set(l, mid, m, x, y, v);
				else ch->set(mid, r, m, x, y, v);
				v = f(lt ? lt->tree.query(0, m, y, y+1) : unit,
					rt ? rt->tree.query(0, m, y, y+1) : unit);
			}
			tree.set(0, m, y, v);
		}
		V query(int l, int r, int m, int a, int b,
			int c, int d) const {
			if (b <= l || r <= a) return unit;
			if (a <= l && r <= b)
				return tree.query(0, m, c, d);
			int mid = l + (r - l) / 2;
			return f(lt ? lt->query(l,mid,m,a,b,c,d) : unit,
				rt ? rt->query(mid,r,m,a,b,c,d) : unit);
		}
		~Node() { delete lt; delete rt; }
	} root;
	int n, m;
	Tree2D(int n, int m) : n(n), m(m) {
		assert(n > 0 && m > 0);
	}
	Tree2D(const Tree2D&) = delete;
	Tree2D& operator=(const Tree2D&) = delete;
	void set(int x, int y, V v) {
		assert(0 <= x && x < n && 0 <= y && y < m);
		root.set(0, n, m, x, y, v);
	}
	V query(int x1, int x2, int y1, int y2) const {
		if (x1 >= x2 || y1 >= y2) return unit;
		return root.query(0, n, m, x1, x2, y1, y2);
	}
};
