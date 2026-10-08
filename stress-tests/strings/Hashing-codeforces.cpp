#include "../utilities/template.h"

#define main main2
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wreturn-type"
#include "../../content/strings/Hashing-codeforces.h"
#pragma GCC diagnostic pop
#undef main

#include <sys/time.h>
void checkBytes(string s) {
	HashInterval hi(s);
	rep(len,0,sz(s)+1) {
		auto hashes = getHashes(s, len);
		assert(sz(hashes) == sz(s)-len+1);
		rep(i,0,sz(hashes)) {
			string sub = s.substr(i, len);
			assert(hashes[i] == hashString(sub));
			assert(hashes[i] == hi.hashInterval(i, i+len));
		}
	}
}
int main() {
	timeval tp;
	gettimeofday(&tp, 0);
	C = (int)tp.tv_usec;
	assert((ull)(H(1)*2+1-3) == 0);
	C = 123456; // deterministic base for byte regressions
	rep(i,0,256) {
		string s(1, (char)i);
		assert(hashString(s) == H(i));
		rep(j,0,256) checkBytes(s + (char)j);
	}
	mt19937 rng(123456);
	rep(it,0,1000) {
		string s(rng()%33, '\0');
		for (char& c : s) c = (char)(rng()%256);
		checkBytes(s);
	}
	C = (int)tp.tv_usec;

	rep(it,0,10000) {
		int n = rand() % 10;
		int alpha = rand() % 10 + 1;
		string s;
		rep(i,0,n) s += (char)('a' + rand() % alpha);
		HashInterval hi(s);
		set<string> strs;
		set<ull> hashes;

		// HashInterval
		rep(i,0,n+1) rep(j,i,n+1) {
			string sub = s.substr(i, j - i);
			ull hash = (ull) hashString(sub);
			assert((ull) hi.hashInterval(i, j) == hash);
			hashes.insert(hash);
			strs.insert(sub);
		}

		// getHashes
		rep(le,1,n+1) {
			auto ve = getHashes(s, le);
			assert(sz(ve) == n-le+1);
			rep(i,0,n-le+1) {
				assert((ull) ve[i] == (ull) hi.hashInterval(i, i + le));
			}
		}

		// No collisions
		assert(sz(strs) == sz(hashes));
	}
	cout<<"Tests passed!"<<endl;
}
