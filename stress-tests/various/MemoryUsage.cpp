#include "../utilities/template.h"

#include "../../content/various/MemoryUsage.h"

#include <sys/mman.h>

static ll currentRssKb() {
	ifstream in("/proc/self/status");
	string key, val;
	while (in >> key >> val) {
		if (key == "VmRSS:") return stoll(val);
	}
	return -1;
}

int main() {
	const size_t N = 32ull << 20; // 32 MiB, touched so it counts toward RSS
	ll peak0 = getMemoryUsage();
	ll rss0 = currentRssKb();

	void* p = mmap(nullptr, N, PROT_READ | PROT_WRITE,
		MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	assert(p != MAP_FAILED);
	memset(p, 1, N);

	ll peak1 = getMemoryUsage();
	ll rss1 = currentRssKb();
	assert(peak1 > peak0);

	munmap(p, N);

	ll peak2 = getMemoryUsage();
	ll rss2 = currentRssKb();
	assert(peak2 >= peak1); // lifetime high-water mark; does not drop

	cout << "ru_maxrss (lifetime peak): " << peak0 << " -> " << peak1
		<< " -> " << peak2 << " (after unmap)" << endl;
	if (rss0 >= 0) {
		cout << "VmRSS kB (current):        " << rss0 << " -> " << rss1
			<< " -> " << rss2 << " (after unmap)" << endl;
		assert(rss1 > rss0);
		assert(rss2 < rss1);
	}

	cout << "Tests passed!" << endl;
}
