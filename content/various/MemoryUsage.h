/**
 * Author: me
 * Date: 2026-08-16
 * License: CC0
 * Source: me
 * Description: Maximum RSS over the lifetime of the program so far, not current RSS. Kilobytes on Linux, bytes on macOS.
 * Status: stress-tested
 */
#pragma once

#include <sys/resource.h> /** keep-include */

ll getMemoryUsage() {
	struct rusage usage;
	getrusage(RUSAGE_SELF, &usage);
	return usage.ru_maxrss;
}
