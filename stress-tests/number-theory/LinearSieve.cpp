#include "../utilities/template.h"

#include "../../content/number-theory/LinearSieve.h"
#include "../../content/number-theory/Eratosthenes.h"

int main() {
	linearSieve();
	vi pr2 = eratosthenesSieve(MAX_PR);
	int k = 0;
	while (k < sz(pr) && pr[k] < MAX_PR) k++;
	assert(vi(pr.begin(), pr.begin() + k) == pr2);

	rep(i,2,SIEVE_N+1) {
		assert(lp[i] >= 2);
		assert(lp[i] <= i);
		assert(i % lp[i] == 0);
		assert(lp[lp[i]] == lp[i]);
		if (lp[i] == i) continue;
		int q = i / lp[i];
		assert(lp[q] >= lp[i]);
	}
	rep(i,2,SIEVE_N+1) {
		if (lp[i] == i) {
			assert(binary_search(all(pr), i));
		}
	}
	assert(lp[2] == 2 && lp[4] == 2 && lp[9] == 3 && lp[1] == 0);
	cout << "Tests passed!" << endl;
}
