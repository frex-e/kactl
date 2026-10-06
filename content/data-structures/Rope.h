/**
 * Author: me
 * License: CC0
 * Source: GNU libstdc++ ext/rope
 * Description: GNU extension for editable sequences.
 *  Copies share storage; later edits leave copies unchanged.
 *  Indices are 0-based; erase/substr take (position, length).
 *  No range aggregates or lazy reversal. Requires libstdc++.
 * Time: Index $O(\log N)$, copy $O(1)$. Edits typically
 *  $O(\log N)$ plus new data, but can take $O(N)$.
 * Status: tested
 */
#pragma once

#include <ext/rope> /** keep-include */
using __gnu_cxx::rope;

void ropeExample() {
	int a[] = {0, 1, 2, 3, 4};
	rope<int> r(a, a + 5); // or rope<int>(n, value)
	rope<int> saved = r; // cheap snapshot
	r.insert(2, rope<int>(3, 10)); // insert before index 2
	r.erase(1, 3); // remove [1, 4): {0, 10, 2, 3, 4}
	r.replace(0, 9); // set index 0 (also r[0] = 9)
	rope<int> sub = r.substr(1, 2); // {10, 2}
	r += sub; // concatenate; r.push_back(x) also works
	vector<int> v(all(r)); // materialize in O(N)
	assert(v == (vi{9, 10, 2, 3, 4, 10, 2}));
	assert(saved[0] == 0 && saved.size() == 5);
	rope<char> s("abc"); // string variant
	s.insert(1, "XY"); // aXYbc
}
