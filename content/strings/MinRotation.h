/**
 * Author: Stjepan Glavina
 * License: Unlicense
 * Source: https://github.com/stjepang/snippets/blob/master/min_rotation.cpp
 * Description: Finds the lexicographically smallest rotation of a string.
 * Bytes are ordered as unsigned, including nul bytes.
 * Time: O(N)
 * Usage:
 *  rotate(v.begin(), v.begin()+minRotation(v), v.end());
 * Status: Stress-tested
 */
#pragma once

int minRotation(string s) {
	int a=0, N=sz(s); s += s;
	rep(b,0,N) rep(k,0,N) {
		auto x = (unsigned char)s[a+k], y = (unsigned char)s[b+k];
		if (a+k == b || x < y) {b += max(0, k-1); break;}
		if (x > y) { a = b; break; }
	}
	return a;
}
