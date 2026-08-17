/**
 * Author: me
 * Date: 2026-08-16
 * License: CC0
 * Source: mine.typ
 * Description: Binary trie over 32-bit values. Supports
 *  insert, XOR-max, and mex of values XOR $xr$.
 *  $size$ is only needed for mex.
 * Time: $O(B)$
 * Status: stress-tested
 */
#pragma once

struct BinaryTrie {
	typedef unsigned U;
	BinaryTrie *nxt[2];
	U size = 0;
	BinaryTrie() { nxt[0] = nxt[1] = 0; }
	bool insert(U x, int ind = 31) {
		if (ind < 0) {
			if (size == 0) return (size = 1);
			return false;
		}
		int bit = (x >> ind) & 1;
		if (!nxt[bit]) nxt[bit] = new BinaryTrie();
		bool res = nxt[bit]->insert(x, ind - 1);
		size += res;
		return res;
	}
	U mex(U xr, U cur = 0, int ind = 31) {
		if (ind < 0) return cur;
		int bit = (xr >> ind) & 1;
		if (nxt[bit] && nxt[bit]->size == (1u << ind)) {
			if (nxt[!bit])
				return nxt[!bit]->mex(xr, cur | (1u << ind),
					ind - 1);
			return cur | (1u << ind);
		} else if (nxt[bit])
			return nxt[bit]->mex(xr, cur, ind - 1);
		return cur;
	}
	U maxxor(U xr, U cur = 0, int ind = 31) {
		if (ind < 0) return cur;
		int bit = (xr >> ind) & 1;
		if (nxt[!bit])
			return nxt[!bit]->maxxor(xr, cur | (1u << ind),
				ind - 1);
		if (nxt[bit])
			return nxt[bit]->maxxor(xr, cur, ind - 1);
		return cur;
	}
	~BinaryTrie() { delete nxt[0]; delete nxt[1]; }
};
