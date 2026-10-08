#include "../utilities/template.h"

#include "../../content/strings/Hashing.h"

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
	assert((H(1)*2+1-3).get() == 0);
	rep(i,0,256) {
		string s(1, (char)i);
		assert(hashString(s).get() == (ull)i);
		rep(j,0,256) checkBytes(s + (char)j);
	}
	mt19937 rng(123456);
	rep(it,0,1000) {
		string s(rng()%33, '\0');
		for (char& c : s) c = (char)(rng()%256);
		checkBytes(s);
	}

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
			ull hash = hashString(sub).get();
			assert(hi.hashInterval(i, j).get() == hash);
			hashes.insert(hash);
			strs.insert(sub);
		}

		// getHashes
		rep(le,1,n+1) {
			auto ve = getHashes(s, le);
			assert(sz(ve) == n-le+1);
			rep(i,0,n-le+1) {
				assert(ve[i].get() == hi.hashInterval(i, i + le).get());
			}
		}

		// No collisions
		assert(sz(strs) == sz(hashes));
	}
	cout<<"Tests passed!"<<endl;
}
