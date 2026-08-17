#include "../utilities/template.h"

#include "../../content/data-structures/BinaryTrie.h"

unsigned bruteMex(const set<unsigned>& vals, unsigned xr) {
	set<unsigned> xored;
	for (unsigned v : vals) xored.insert(v ^ xr);
	unsigned m = 0;
	while (xored.count(m)) m++;
	return m;
}

unsigned bruteMaxxor(const set<unsigned>& vals, unsigned xr) {
	unsigned best = 0;
	for (unsigned v : vals) best = max(best, v ^ xr);
	return best;
}

unsigned randU() {
	return (unsigned)rand() ^ ((unsigned)rand() << 15);
}

void testRandom(int n, int queries) {
	BinaryTrie t;
	set<unsigned> vals;
	rep(i,0,n) {
		unsigned x = randU();
		bool added = t.insert(x);
		assert(added == !vals.count(x));
		vals.insert(x);
		assert(t.size == (unsigned)sz(vals));
	}
	rep(q,0,queries) {
		unsigned xr = rand() % 2 ? randU() : (unsigned)(rand() % 32);
		assert(t.mex(xr) == bruteMex(vals, xr));
		assert(t.maxxor(xr) == bruteMaxxor(vals, xr));
	}
}

void testConsecutive() {
	rep(k,0,200) {
		BinaryTrie t;
		rep(i,0,k) assert(t.insert((unsigned)i));
		assert(t.mex(0) == (unsigned)k);
		if (k)
			assert(t.maxxor(0) == (unsigned)(k - 1));
		else
			assert(t.maxxor(0) == 0);
		rep(xr,0,min(k + 3, 20)) {
			set<unsigned> vals;
			rep(i,0,k) vals.insert((unsigned)i);
			assert(t.mex((unsigned)xr) == bruteMex(vals, (unsigned)xr));
			assert(t.maxxor((unsigned)xr) == bruteMaxxor(vals, (unsigned)xr));
		}
	}
}

void testDuplicatesAndEmpty() {
	BinaryTrie t;
	assert(t.mex(0) == 0);
	assert(t.maxxor(0) == 0);
	assert(t.insert(0));
	assert(!t.insert(0));
	assert(t.size == 1);
	assert(t.mex(0) == 1);
	assert(t.insert(2));
	assert(t.mex(0) == 1);
	assert(t.insert(1));
	assert(t.mex(0) == 3);
	assert(t.maxxor(0) == 2);
	assert(t.maxxor(1) == 3);
}

int main() {
	testDuplicatesAndEmpty();
	testConsecutive();
	rep(n,0,40) testRandom(n, 50);
	rep(it,0,20) testRandom(200, 30);
	cout << "Tests passed!" << endl;
}
