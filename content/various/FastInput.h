/**
 * Author: chilli
 * License: CC0
 * Source: Own work
 * Description: Read an integer from stdin. Usage requires your program to pipe in
 * input from file. Supports the full signed int range.
 * Usage: ./a.out < input.txt
 * Time: About 5x as fast as cin/scanf.
 * Status: tested on SPOJ INTEST, stress-tested
 */
#pragma once

inline char gc() { // like getchar()
	static char buf[1 << 16];
	static size_t bc, be;
	if (bc >= be) {
		buf[0] = 0, bc = 0;
		be = fread(buf, 1, sizeof(buf), stdin);
	}
	return buf[bc++]; // returns 0 on EOF
}

int readInt() {
	ll a = 0;
	int c;
	while ((c = gc()) < 40);
	bool neg = c == '-';
	if (neg) c = gc();
	do { a = a * 10 + c - '0'; } while ((c = gc()) >= 48);
	return int(neg ? -a : a);
}
