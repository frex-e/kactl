#include "../utilities/template.h"

#include "../../content/data-structures/BinaryTrie.h"

int bruteMex(const set<int>& vals, int xr) {
	set<int> xored;
	for (int v : vals) xored.insert(v ^ xr);
	int m = 0;
	while (xored.count(m)) m++;
	return m;
}

int bruteMinxor(const multiset<int>& vals, int xr) {
	if (vals.empty()) return 0;
	int best = INT_MAX;
	for (int v : vals) best = min(best, v ^ xr);
	return best;
}

int bruteMaxxor(const multiset<int>& vals, int xr) {
	if (vals.empty()) return 0;
	int best = 0;
	for (int v : vals) best = max(best, v ^ xr);
	return best;
}

int bruteCount(const multiset<int>& vals, int xr, int k, int sgn) {
	int c = 0;
	for (int v : vals) {
		int x = v ^ xr;
		c += sgn ? x > k : x < k;
	}
	return c;
}

int randVal() { return rand() & ((1 << 20) - 1); }

void checkSet(BinaryTrie& t, const set<int>& vals, int xr) {
	assert(t.cnt == sz(vals));
	assert(t.mex(xr) == bruteMex(vals, xr));
	multiset<int> ms(all(vals));
	assert(t.minxor(xr) == bruteMinxor(ms, xr));
	assert(t.maxxor(xr) == bruteMaxxor(ms, xr));
	rep(it,0,8) {
		int k = randVal();
		assert(t.count<0>(xr, k) == bruteCount(ms, xr, k, 0));
		assert(t.count<1>(xr, k) == bruteCount(ms, xr, k, 1));
	}
}

void testSetOps() {
	rep(it,0,80) {
		BinaryTrie t;
		set<int> vals;
		rep(q,0,80) {
			int x = rand() % 64;
			if (rand() % 3 == 0 && !vals.empty()) {
				int y = *next(vals.begin(), rand() % sz(vals));
				int sub = t.erase(y);
				assert(sub == 1);
				vals.erase(y);
			} else {
				int add = t.insert(x);
				assert(add == !vals.count(x));
				vals.insert(x);
			}
			int xr = rand() % 2 ? rand() % 32 : 0;
			checkSet(t, vals, xr);
		}
	}
}

void testMulti() {
	rep(it,0,40) {
		BinaryTrie t;
		multiset<int> vals;
		rep(q,0,60) {
			int x = rand() % 32;
			if (rand() % 3 == 0 && !vals.empty()) {
				int y = *next(vals.begin(), rand() % sz(vals));
				assert(t.erase(y) == 1);
				vals.erase(vals.find(y));
			} else {
				t.insertMulti(x);
				vals.insert(x);
			}
			assert(t.cnt == sz(vals));
			int xr = rand() % 16;
			assert(t.minxor(xr) == bruteMinxor(vals, xr));
			assert(t.maxxor(xr) == bruteMaxxor(vals, xr));
			int k = rand() % 64;
			assert(t.count<0>(xr, k) == bruteCount(vals, xr, k, 0));
			assert(t.count<1>(xr, k) == bruteCount(vals, xr, k, 1));
		}
	}
}

void testXorAllAndMex() {
	rep(k,0,80) {
		BinaryTrie t;
		set<int> vals;
		rep(i,0,k) {
			assert(t.insert(i));
			vals.insert(i);
		}
		assert(t.mex() == k);
		rep(xr,0,min(k + 5, 20)) {
			assert(t.mex(xr) == bruteMex(vals, xr));
			assert(t.maxxor(xr) == bruteMaxxor(multiset<int>(all(vals)), xr));
		}
	}
	BinaryTrie t;
	assert(t.mex() == 0);
	assert(t.minxor(0) == 0);
	assert(t.maxxor(0) == 0);
	t.insert(1); t.insert(2); t.insert(3);
	t.xorAll(1);
	assert(t.mex() == 0);
	assert(t.minxor(0) == 0);
	assert(t.maxxor(0) == 2);
	t.xorAll(1);
	assert(t.mex() == 0);
	assert(t.maxxor(0) == 3);
}

void testMerge() {
	rep(it,0,50) {
		BinaryTrie a, b;
		set<int> vals;
		rep(i,0,40) {
			int x = randVal();
			if (rand() % 2) {
				if (a.insert(x)) vals.insert(x);
			} else {
				if (b.insert(x)) vals.insert(x);
			}
		}
		a.merge(b);
		assert(b.cnt == 0);
		checkSet(a, vals, randVal());
		checkSet(a, vals, 0);
	}
}

void testDuplicatesAndEmpty() {
	BinaryTrie t;
	assert(t.mex(0) == 0);
	assert(t.maxxor(0) == 0);
	assert(t.insert(0));
	assert(!t.insert(0));
	assert(t.cnt == 1);
	assert(t.mex(0) == 1);
	assert(t.insert(2));
	assert(t.mex(0) == 1);
	assert(t.insert(1));
	assert(t.mex(0) == 3);
	assert(t.maxxor(0) == 2);
	assert(t.maxxor(1) == 3);
	assert(t.minxor(0) == 0);
	assert(t.minxor(7) == 4);
	assert(t.erase(1));
	assert(!t.erase(1));
	assert(t.cnt == 2);
}

int main() {
	testDuplicatesAndEmpty();
	testSetOps();
	testMulti();
	testXorAllAndMex();
	testMerge();
	cout << "Tests passed!" << endl;
}
