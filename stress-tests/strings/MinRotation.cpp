#include "../utilities/template.h"

#include "../../content/strings/MinRotation.h"

int min_rotation2(string& v) {
	int n = sz(v);
	string w = v; w.insert(w.end(), all(v));
	int j = 0;
	rep(i,1,n) {
		if (w.compare(i, n, w, j, n) < 0) j = i;
	}
	return j;
}

void testPerf() {
	string s;
	rep(i,0,10000000)
		s += (char)(rand()%400000 < 2);
	cout << minRotation(s) << endl;
}

int main() {
	rep(a,0,256) rep(b,0,256) {
		string v{char(a), char(b)};
		assert(minRotation(v) == min_rotation2(v));
	}
	rep(it,0,1000000) {
		int n = rand() % 10;
		string v;
		rep(i,0,n) v += (char)(rand() % (it & 1 ? 256 : 3));
		int r = minRotation(v);
		int r2 = min_rotation2(v);
		assert(r == r2);
		rotate(v.begin(), v.begin() + r, v.end());
		assert(minRotation(v) == 0);
		assert(min_rotation2(v) == 0);
	}
	cout<<"Tests passed!"<<endl;
}
