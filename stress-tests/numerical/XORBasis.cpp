#include "../utilities/template.h"

#include "../../content/numerical/XORBasis.h"

struct BitBasis32 {
	unsigned b[32]{};
	bool have[32]{};
	void add(unsigned x) {
		for (int i = 31; i >= 0; i--) if ((x >> i) & 1) {
			if (!have[i]) { b[i] = x; have[i] = true; return; }
			x ^= b[i];
		}
	}
	bool inSpan(unsigned x) {
		for (int i = 31; i >= 0; i--) if ((x >> i) & 1) {
			if (!have[i]) return false;
			x ^= b[i];
		}
		return true;
	}
	int size() {
		int n = 0;
		rep(i,0,32) n += have[i];
		return n;
	}
};

void xorOne(vector<uint64_t>& a, const vector<uint64_t>& b) {
	rep(i,0,sz(a)) a[i] ^= b[i];
}

struct BitBasisBig {
	int B;
	vector<vector<uint64_t>> have;
	BitBasisBig(int bits) : B(bits), have(bits) {}
	bool get(const vector<uint64_t>& x, int i) {
		return (x[i / 64] >> (i % 64)) & 1;
	}
	void add(vector<uint64_t> x) {
		for (int i = B - 1; i >= 0; i--) if (get(x, i)) {
			if (have[i].empty()) { have[i] = x; return; }
			xorOne(x, have[i]);
		}
	}
	bool inSpan(vector<uint64_t> x) {
		for (int i = B - 1; i >= 0; i--) if (get(x, i)) {
			if (have[i].empty()) return false;
			xorOne(x, have[i]);
		}
		return true;
	}
	int size() {
		int n = 0;
		rep(i,0,B) n += !have[i].empty();
		return n;
	}
};

unsigned randU() {
	return (unsigned)rand() ^ ((unsigned)rand() << 15);
}

void testXorBasis() {
	rep(it,0,500) {
		XorBasis xb;
		BitBasis32 ref;
		vi inserted;
		int n = rand() % 40;
		rep(i,0,n) {
			int x = (int)(randU() & 0x7fffffff);
			xb.add(x);
			ref.add((unsigned)x);
			inserted.push_back(x);
			assert(sz(xb.basis) == ref.size());
		}
		assert(xb.inSpan(0));
		rep(q,0,80) {
			int x = (int)(randU() & 0x7fffffff);
			assert(xb.inSpan(x) == ref.inSpan((unsigned)x));
		}
		for (int x : inserted) assert(xb.inSpan(x));
	}
}

vector<uint64_t> randVec(int limbs) {
	vector<uint64_t> x(limbs);
	rep(i,0,limbs) {
		x[i] = (uint64_t)randU() | ((uint64_t)randU() << 32);
	}
	return x;
}

void testBigBasis() {
	const int LIMBS = 2, BITS = 128;
	rep(it,0,200) {
		BigBasis xb;
		BitBasisBig ref(BITS);
		int n = rand() % 25;
		vector<vector<uint64_t>> inserted;
		rep(i,0,n) {
			auto x = randVec(LIMBS);
			xb.add(x);
			ref.add(x);
			inserted.push_back(x);
			assert(sz(xb.basis) == ref.size());
		}
		rep(q,0,40) {
			auto x = randVec(LIMBS);
			bool got = !nonzero(xb.reduce(x));
			assert(got == ref.inSpan(x));
		}
		for (auto& x : inserted)
			assert(!nonzero(xb.reduce(x)));
		BigBasis xb2;
		shuffle(all(inserted), mt19937(it));
		for (auto& x : inserted) xb2.add(x);
		assert(xb.equal(xb2));
		assert(xb2.equal(xb));
	}
}

int main() {
	XorBasis empty;
	assert(empty.inSpan(0));
	assert(!empty.inSpan(1));
	testXorBasis();
	testBigBasis();
	cout << "Tests passed!" << endl;
}
