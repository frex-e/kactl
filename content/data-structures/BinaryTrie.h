/**
 * Author: caterpillow, me
 * Date: 2026-08-25
 * License: CC0
 * Source: https://github.com/caterpillow/cactl Trie.h
 * Description: Binary trie on $[0,2^B)$. Set \texttt{insert}/
 *  \texttt{erase}, multiset \texttt{insertMulti}, min/max,
 *  count $x<k$ (\texttt{count<0>}) or $>k$
 *  (\texttt{count<1>}), lazy XOR-all, mex (set). Query
 *  $v\oplus x$ via \texttt{xorAll}. \texttt{cnt} is the size.
 *  \texttt{merge} is set-union; \texttt{merge<1>} adds counts.
 *  mex needs unique values.
 * Time: $O(B)$ per op
 * Status: stress-tested
 */
#pragma once

struct BinaryTrie {
	using T = BinaryTrie;
	static const int B = 30;
	int cnt = 0, lazy = 0;
	T *c[2] = {};
	T*& ch(int b) {
		return c[b] ? c[b] : c[b] = new T();
	}
	int cc(int b) { return c[b] ? c[b]->cnt : 0; }
	void push(int i) {
		if (!lazy) return;
		if (i && (lazy >> (i - 1) & 1)) swap(c[0], c[1]);
		rep(b,0,2) if (c[b]) c[b]->lazy ^= lazy;
		lazy = 0;
	}
	void xorAll(int x) { lazy ^= x; }
	int insert(int x, int i = B) {
		push(i);
		if (!i) return cnt ? 0 : (cnt = 1);
		int b = x >> --i & 1;
		int add = ch(b)->insert(x, i);
		return cnt += add, add;
	}
	void insertMulti(int x, int i = B) {
		push(i);
		if (!i) { cnt++; return; }
		int b = x >> --i & 1;
		ch(b)->insertMulti(x, i);
		cnt++;
	}
	int erase(int x, int i = B) {
		if (!cnt) return 0;
		push(i);
		int sub = 1;
		if (i) {
			int b = x >> --i & 1;
			sub = c[b] ? c[b]->erase(x, i) : 0;
		}
		return cnt -= sub, sub;
	}
	int minxor(int i = B) {
		if (!i || !cnt) return 0;
		push(i);
		--i;
		return cc(0) ? c[0]->minxor(i) :
			c[1]->minxor(i) | 1 << i;
	}
	int maxxor(int i = B) {
		if (!i || !cnt) return 0;
		push(i);
		--i;
		return cc(1) ? c[1]->maxxor(i) | 1 << i :
			c[0]->maxxor(i);
	}
	template<int sgn = 0>
	int count(int k, int i = B) {
		if (!i || !cnt) return 0;
		push(i);
		int b = k >> --i & 1;
		return (b ^ sgn ? cc(!b) : 0) +
			(c[b] ? c[b]->count<sgn>(k, i) : 0);
	}
	int mex(int i = B) {
		if (!i) return 0;
		push(i);
		--i;
		if (cc(0) == 1 << i)
			return (c[1] ? c[1]->mex(i) : 0) | 1 << i;
		return c[0] ? c[0]->mex(i) : 0;
	}
	template<int ms = 0>
	void merge(T& o, int i = B) {
		push(i); o.push(i);
		if (!o.cnt) return;
		if (!cnt) {
			swap(c[0], o.c[0]); swap(c[1], o.c[1]);
			swap(cnt, o.cnt); swap(lazy, o.lazy);
			return;
		}
		if (!i) {
			cnt = ms ? cnt + o.cnt : 1;
			o.cnt = 0; return;
		}
		rep(b,0,2) {
			if (!c[b]) c[b] = o.c[b], o.c[b] = 0;
			else if (o.c[b]) c[b]->merge<ms>(*o.c[b], i - 1);
		}
		cnt = cc(0) + cc(1);
		o.cnt = 0;
	}
	~BinaryTrie() { delete c[0]; delete c[1]; }
};
