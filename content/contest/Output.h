/**
 * Author: me
 * Date: 2026-08-17
 * License: CC0
 * Source: me
 * Description: Output formatting with streams,
 *  C++20 \texttt{std::format}, and \texttt{printf}.
 *  Field width is a minimum.
 * Status: examples checked
 */
#pragma once

#include <format> /// keep-include

// Binary: format needs C++20 library support.
// cout << format("{:b}", 5); // 101 /// include-line
// cout << format("{:08b}", 5); // 00000101 /// include-line
// cout << bitset<8>(5); // 00000101, low 8 bits /// include-line
// bitset<N> prints exactly N bits; N is compile-time.

// Streams: precision/fill/alignment persist; setw is once.
// cout << fixed << setprecision(2) << 1.5; // 1.50 /// include-line
// cout << defaultfloat << setprecision(3) << 1.2345; /// include-line
// Above: 1.23 (defaultfloat uses significant digits).
// cout << setfill('0') << setw(5) << 42; // 00042 /// include-line
// cout << setfill(' ') << left << setw(5) << "hi"; /// include-line
// Above: "hi   "; restore right for right alignment.
// cout << right; /// include-line

// format: padding/alignment, decimal places, runtime width.
// cout << format("{:05}", 42); // 00042 /// include-line
// cout << format("{:<5}", "hi"); // "hi   " /// include-line
// cout << format("{:8.2f}", 1.5); // "    1.50" /// include-line
// cout << format("{:.{}f}", 1.5, 3); // 1.500 /// include-line
// cout << format("{:0{}b}", 5, 8); // 00000101 /// include-line

// printf: int %d, unsigned %u, long long %lld,
// unsigned long long %llu, C string %s, char %c.
// float promotes to double: %f; long double: %Lf.
// int i = 42; unsigned u = 7; ll n = 12345678901LL; /// include-line
// string s = "hi"; double x = 1.5; long double y = 2.5L; /// include-line
// printf("%d %u %lld\n", i, u, n); /// include-line
// printf("%s %c\n", s.c_str(), 'A'); /// include-line
// printf("%.2f %.2Lf\n", x, y); // 1.50 2.50 /// include-line
// printf("%05d\n", i); // 00042 (sign before zeros) /// include-line
// printf("%-5s!\n", s.c_str()); // "hi   !" /// include-line
// printf("%8.2f\n", x); // "    1.50" /// include-line
// printf("%*.*f\n", 8, 2, x); // runtime width/precision /// include-line
// %e: scientific; %.3g: 3 significant digits; %%: literal %.
// With sync_with_stdio(false), don't mix cout and printf.
