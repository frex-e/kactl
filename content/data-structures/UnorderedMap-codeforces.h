/**
 * Author: ChatGPT
 * Description: Codeforces hash map for integer keys, with a
 * randomized SplitMix64 hash against Codeforces hacks.
 * Seed is fixed per run; no worst-case guarantee.
 * Usage: unordered_map<ll, int, RandomHash> h;
 * Time: Expected O(1) per operation.
 */
#pragma once

struct RandomHash {
	static uint64_t mix(uint64_t x) {
		x += 0x9e3779b97f4a7c15;
		x = (x ^ (x >> 30)) * 0xbf58476d1ce4e5b9;
		x = (x ^ (x >> 27)) * 0x94d049bb133111eb;
		return x ^ (x >> 31);
	}
	size_t operator()(uint64_t x) const {
		static const uint64_t seed = chrono::steady_clock::now()
			.time_since_epoch().count();
		return mix(x + seed);
	}
};
unordered_map<ll, int, RandomHash> h;
