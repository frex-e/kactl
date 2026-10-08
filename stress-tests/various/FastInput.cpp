#include "../utilities/template.h"
#include <unistd.h>
#include "../../content/various/FastInput.h"

constexpr int BUF_SIZE = 1 << 16;

string tempdirname;
string tempfilename;

void test(const string& s, vi ints = {}) {
	ofstream fout(tempfilename);
	fout << s;
	fout.close();
	FILE* ret = freopen(tempfilename.c_str(), "r", stdin);
	assert(ret == stdin);
	if (ints.empty()) {
		for (char c : s) {
			int c2 = gc();
			assert(c == c2);
		}
		assert(gc() == 0);
		assert(gc() == 0);
	} else {
		for (int x : ints) {
			int y = readInt();
			if (x != y) {
				cerr << "On input " << s << ", read " << y << " but expected " << x << endl;
			}
			assert(x == y);
		}
		// Drain trailing whitespace before reopening stdin.
		while (gc());
	}
}

int main() {
	// Unit test, not stress test, but oh well.
	char pattern[] = "/tmp/fastinputXXXXXX";
	tempdirname = mkdtemp(pattern);
	tempfilename = tempdirname + "/stdin.txt";

	// First test that the getchar implementation is correct:
	test("");
	test("a");
	test("ab");
	string s;
	for (int i = 0; i < BUF_SIZE; i++) s += (char)(i % 13);
	test(s);
	for (int i = 0; i < BUF_SIZE * 10 + 1; i++) s += (char)(i % 13);
	test(s);
	for (int i = 0; i < BUF_SIZE - 2; i++) s += (char)(i % 13);
	test(s);
	for (int i = 0; i < BUF_SIZE + 2; i++) {
		assert(gc() == 0);
	}

	// Then test that readInt() is:
	test("1", {1});
	test("12", {12});
	test("9\n", {9});
	test("12 ", {12});
	test("-23\n", {-23});
	test(" -4", {-4});
	test(" 5\n", {5});
	test("1 -2 ", {1, -2});
	test("  -34   56   ", {-34, 56});
	test(" \t\r\n5 -2 ", {5});
	test("2147483647 -2147483648", {INT_MAX, INT_MIN});
	test("2147483646 -2147483647 0 -0 00012",
		{INT_MAX-1, INT_MIN+1, 0, 0, 12});
	test(string(BUF_SIZE-1, ' ') + "-2147483648", {INT_MIN});
	test(string(BUF_SIZE-5, ' ') + "2147483647", {INT_MAX});
	mt19937 rng(20261008);
	uniform_int_distribution<int> value(INT_MIN, INT_MAX);
	vi ints;
	s.clear();
	rep(i,0,20000) {
		int x = value(rng);
		ints.push_back(x);
		s += to_string(x) + (i % 2 ? "\n" : " \t");
	}
	test(s, ints);

	unlink(tempfilename.c_str());
	rmdir(tempdirname.c_str());
	cout << "Tests passed!" << endl;
}
