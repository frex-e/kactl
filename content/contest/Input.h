/**
 * Author: me
 * Date: 2026-10-07
 * License: CC0
 * Source: me
 * Description: \texttt{scanf} reference. Arguments are
 *  pointers to the exact types below. The return value
 *  is the number of assignments, or EOF if input ends
 *  before the first conversion. Check it before use.
 * Status: examples checked
 */
#pragma once

// int: %d (decimal), unsigned: %u, long long: %lld,
// unsigned long long: %llu. %i auto-detects octal/hex.
// int i; unsigned u; ll n; /// include-line
// if (scanf("%d %u %lld", &i, &u, &n) != 3) return 0; /// include-line

// Unlike printf: %f needs float*, %lf needs double*.
// float f; double x; long double y; /// include-line
// if (scanf("%f %lf %Lf", &f, &x, &y) != 3) return 0; /// include-line

// %s reads a token; bound width to buffer size minus 1.
// char s[100]; /// include-line
// if (scanf("%99s", s) != 1) return 0; // no &s /// include-line
// It appends '\0'; don't pass std::string to scanf.
// char c; /// include-line
// if (scanf(" %c", &c) != 1) return 0; /// include-line
// %c alone reads whitespace too; leading space skips it.
// Format whitespace consumes any amount of whitespace;
// avoid trailing spaces or '\n' (can wait for more input).
// With sync_with_stdio(false), don't mix cin and scanf.
