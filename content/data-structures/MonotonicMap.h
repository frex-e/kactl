/**
 * Author: caterpillow
 * Date: 2025-10-21
 * License: CC0
 * Source: https://github.com/caterpillow/cactl
 *  MonotonicMap.h
 * Description: Prefix/suffix min/max queries with
 *  insertions. Useful replacement for sparse segtrees.
 * Time: $O(\log N)$
 * Status: stress-tested
 */
#pragma once

// dir = less<> for suffix queries, greater<> for prefix
// cmp = less_equal<> for min, greater_equal<> for max
template<class dir, class cmp>
struct RangeQuery {
	map<int, ll, dir> data;
	void ins(int k, ll v) {
		if (auto it = data.lower_bound(k);
			it != data.end() && cmp{}(it->second, v))
			return;
		auto it = data.insert_or_assign(k, v).first;
		while (it != data.begin()
			&& cmp{}(v, prev(it)->second))
			data.erase(prev(it));
	}
	ll query(int k) { // inclusive: careful UB, insert infinity
		return data.lower_bound(k)->second;
	}
};
