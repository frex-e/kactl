/**
 * Author: 罗穗骞, chilli
 * Date: 2019-04-11
 * License: Unknown
 * Source: Suffix array - a powerful tool for dealing with strings
 * (Chinese IOI National team training paper, 2009)
 * Description: Builds suffix array for a string.
 * \texttt{sa[i]} is the starting index of the suffix which
 * is $i$'th in the sorted suffix array.
 * The returned vector is of size $n+1$, and \texttt{sa[0] = n}.
 * The \texttt{lcp} array contains longest common prefixes for
 * neighbouring strings in the suffix array:
 * \texttt{lcp[i] = lcp(sa[i], sa[i-1])}, \texttt{lcp[0] = 0}.
 * Also stores \texttt{rank} (inverse SA) and an RMQ table on
 * LCP so \texttt{getLCP(i,j)} / \texttt{cmpSubstr} are
 * $O(1)$. The input string must not contain any nul chars.
 * Time: O(n \log n) build
 * Status: stress-tested
 */
#pragma once

struct SuffixArray {
	vi sa, lcp, rank, lg;
	vector<vi> st;
	SuffixArray(string s, int lim = 256) { // or vector<int>
		s.push_back(0); int n = sz(s), k = 0, a, b;
		vi x(all(s)), y(n), ws(max(n, lim));
		sa = lcp = y, iota(all(sa), 0);
		for (int j = 0, p = 0; p < n; j = max(1, j * 2), lim = p) {
			p = j, iota(all(y), n - j);
			rep(i,0,n) if (sa[i] >= j) y[p++] = sa[i] - j;
			fill(all(ws), 0);
			rep(i,0,n) ws[x[i]]++;
			rep(i,1,lim) ws[i] += ws[i - 1];
			for (int i = n; i--;) sa[--ws[x[y[i]]]] = y[i];
			swap(x, y), p = 1, x[sa[0]] = 0;
			rep(i,1,n) a = sa[i - 1], b = sa[i], x[b] =
				(y[a] == y[b] && y[a + j] == y[b + j]) ? p - 1 : p++;
		}
		for (int i = 0, j; i < n - 1; lcp[x[i++]] = k)
			for (k && k--, j = sa[x[i] - 1];
					s[i + k] == s[j + k]; k++);
		rank = x;
		buildRMQ();
	}
	void buildRMQ() {
		int n = sz(lcp);
		lg.assign(n + 1, 0);
		rep(i,2,n+1) lg[i] = lg[i / 2] + 1;
		st.assign(lg[n] + 1, vi(n));
		st[0] = lcp;
		rep(k,1,sz(st)) rep(i,0,n - (1 << k) + 1)
			st[k][i] = min(st[k-1][i], st[k-1][i + (1<<(k-1))]);
	}
	int rmq(int l, int r) { // inclusive
		if (l > r) return INT_MAX;
		int k = lg[r - l + 1];
		return min(st[k][l], st[k][r - (1 << k) + 1]);
	}
	int getLCP(int i, int j) {
		if (i == j) return sz(sa) - 1 - i; // ignore sentinel
		int ri = rank[i], rj = rank[j];
		if (ri > rj) swap(ri, rj);
		return rmq(ri + 1, rj);
	}
	// compare s[a..a+lena) vs s[b..b+lenb); -1 / 0 / 1
	int cmpSubstr(int a, int lena, int b, int lenb,
			const string& s) {
		int common = min(getLCP(a, b), min(lena, lenb));
		if (common == min(lena, lenb)) {
			if (lena == lenb) return 0;
			return lena < lenb ? -1 : 1;
		}
		return s[a + common] < s[b + common] ? -1 : 1;
	}
};
