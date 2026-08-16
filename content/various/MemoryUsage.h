/**
 * Author: me
 * Date: 2026-08-16
 * License: CC0
 * Source: mine.typ
 * Description: Max RSS. Kilobytes on Linux, bytes on macOS.
 * Status: untested
 */
#pragma once

#include <sys/resource.h> /** keep-include */

ll getMemoryUsage() {
	struct rusage usage;
	getrusage(RUSAGE_SELF, &usage);
	return usage.ru_maxrss;
}
