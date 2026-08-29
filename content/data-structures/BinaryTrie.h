/**
 * Author: caterpillow, me
 * Date: 2026-08-28
 * License: CC0
 * Source: https://github.com/caterpillow/cactl Trie.h
 * Description: Binary trie on $[0,2^B)$. Set \texttt{insert}/
 *  \texttt{erase}, multiset \texttt{insert<1>}, XOR-min/max,
 *  count $x\oplus y<k$ (\texttt{count<0>}) or $>k$
 *  (\texttt{count<1>}), lazy XOR-all, mex (set).
 *  XOR queries take $xr$ (default 0). \texttt{cnt} is the
 *  size. \texttt{each}($f$) calls $f(x,\texttt{cnt})$. \texttt{merge}
 *  is set-union (destroys $o$; safe to delete);
 *  \texttt{merge<1>} adds counts. mex needs unique values.
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
	template<int ms = 0>
	int insert(int x, int i = B) {
		push(i);
		if (!i) return !ms && cnt ? 0 : (++cnt, 1);
		int b = x >> --i & 1;
		int add = ch(b)->insert<ms>(x, i);
		return cnt += add, add;
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
	int minxor(int xr = 0, int i = B) {
		if (!i || !cnt) return 0;
		push(i);
		int b = xr >> --i & 1;
		return cc(b) ? c[b]->minxor(xr, i) :
			c[!b]->minxor(xr, i) | 1 << i;
	}
	int maxxor(int xr = 0, int i = B) {
		if (!i || !cnt) return 0;
		push(i);
		int b = (xr >> --i & 1) ^ 1;
		return cc(b) ? c[b]->maxxor(xr, i) | 1 << i :
			c[!b]->maxxor(xr, i);
	}
	template<int sgn = 0>
	int count(int xr = 0, int k = 0, int i = B) {
		if (!i || !cnt) return 0;
		push(i);
		int b = (xr ^ k) >> --i & 1;
		return ((k >> i & 1) ^ sgn ? cc(!b) : 0) +
			(c[b] ? c[b]->count<sgn>(xr, k, i) : 0);
	}
	int mex(int xr = 0, int i = B) { // unique values
		if (!i) return 0;
		push(i);
		int b = xr >> --i & 1;
		if (cc(b) == 1 << i)
			return (c[!b] ? c[!b]->mex(xr, i) : 0) | 1 << i;
		return c[b] ? c[b]->mex(xr, i) : 0;
	}
	template<class F>
	void each(F f, int i = B, int x = 0) {
		if (!cnt) return;
		push(i);
		if (!i) f(x, cnt);
		else rep(b,0,2) if (c[b])
			c[b]->each(f, i - 1, x | (b << (i - 1)));
	}
	template<int ms = 0>
	void merge(T& o, int i = B) { // o safe to delete
		if (!o.cnt) return;
		push(i); o.push(i);
		if (i) rep(b,0,2) {
			if (!c[b]) swap(c[b], o.c[b]);
			else if (o.c[b]) c[b]->merge<ms>(*o.c[b], i - 1);
		}
		cnt = i ? cc(0) + cc(1) : (ms ? cnt + o.cnt : 1);
		o.cnt = 0;
	}
	~BinaryTrie() { delete c[0]; delete c[1]; }
};
