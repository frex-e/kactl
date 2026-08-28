/**
 * Author: me
 * Date: 2026-08-16
 * License: CC0
 * Source: me
 * Description: Seeded RNG and drawing random integers.
 *  For string hashing, take a random odd 61-bit base.
 * Usage: shuffle(all(v), rng);
 * Status: untested
 */
#pragma once

mt19937_64 rng((uint64_t)chrono::steady_clock::now()
	.time_since_epoch().count());

ll randll(ll l, ll r) { // inclusive
	return uniform_int_distribution<ll>(l, r)(rng);
}

// shuffle(all(v), rng);
// Odd 61-bit hash base:
// ll base = randll(256, (1LL << 61) - 2) | 1;
