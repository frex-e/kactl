/**
 * Author: cp-algorithms
 * Date: 2026-08-28
 * License: CC0
 * Source: https://cp-algorithms.com/string/lyndon_factorization.html
 * Description: Duval's algorithm. A Lyndon word is strictly smaller
 *  than every nonempty proper suffix. Returns $0=a_0<\dots<a_k=n$
 *  so $s[a_i,a_{i+1})$ is the unique non-increasing Lyndon
 *  factorization of $s$.
 * Usage: vi f = duval(s); // factor i is s[f[i], f[i+1])
 * Time: $O(N)$
 * Status: stress-tested
 */
#pragma once

vi duval(const string& s) {
	int n = sz(s), i = 0;
	vi f{0};
	while (i < n) {
		int j = i + 1, k = i;
		while (j < n && s[k] <= s[j]) {
			if (s[k] < s[j]) k = i;
			else k++;
			j++;
		}
		while (i <= k) {
			i += j - k;
			f.pb(i);
		}
	}
	return f;
}
