/**
 * Author: Codex
 * Description: Min Cartesian tree with array indices in
 * inorder. Returns parent, left and right child indices;
 * -1 means no parent/child.
 * Equal values favor the leftmost index. Empty input is OK.
 * Remove p, l or r and its assignments if not needed.
 * Usage: auto [p, l, r] = cartesianTree(a);
 * Time: $O(n)$
 * Status: stress-tested
 */
#pragma once

array<vi, 3> cartesianTree(const vi& a) {
	vi p(sz(a), -1), l = p, r = p, s;
	rep(i,0,sz(a)) {
		int last = -1;
		while (!s.empty() && a[s.back()] > a[i]) {
			last = s.back();
			s.pop_back();
		}
		if (!s.empty()) {
			p[i] = s.back();
			r[s.back()] = i;
		}
		if (last != -1) p[last] = i;
		l[i] = last;
		s.push_back(i);
	}
	return {p, l, r};
}
