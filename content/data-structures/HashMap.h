/**
 * Author: Simon Lindholm, chilli
 * Date: 2018-07-23
 * License: CC0
 * Source: http://codeforces.com/blog/entry/60737
 * Description: Hash map with mostly the same API as \texttt{unordered\_map}, but \tilde
 * 3x faster. Uses 1.5x memory.
 * Initial capacity must be a power of 2 (if provided).
 * For a standard map, use the commented alternative below.
 * Randomization is optional; set RANDOM to 0 to disable it.
 */
#pragma once

#include <bits/extc++.h> /** keep-include */
const uint64_t RANDOM = chrono::steady_clock::now()
	.time_since_epoch().count();
// To use most bits rather than just the lowest ones:
struct chash { // large odd number for C
	const uint64_t C = ll(4e18 * acos(0)) | 71;
	size_t operator()(uint64_t x) const {
		return __builtin_bswap64((x ^ RANDOM) * C);
	}
};
__gnu_pbds::gp_hash_table<ll,int,chash> h({},{},{},{},{1<<16});
// unordered_map<ll, int, chash> h; // alternative
