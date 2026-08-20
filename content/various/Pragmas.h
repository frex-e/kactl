/**
 * Author: me
 * Date: 2026-08-16
 * License: CC0
 * Source: me
 * Description: GCC pragmas. \texttt{Ofast} implies
 *  \texttt{-ffast-math} (can change floats).
 *  \texttt{avx2} can crash on old machines.
 *  \texttt{bmi,bmi2}: tzcnt/pdep/pext.
 *  \texttt{lzcnt,popcnt}: hardware clz/popcount.
 *  \texttt{trapv} aborts on signed overflow (slow; debug).
 *  Do not combine \texttt{trapv} with \texttt{Ofast} if you need traps.
 * Status: untested
 */
#pragma once

#pragma GCC optimize("Ofast,unroll-loops")
#pragma GCC target("avx2,bmi,bmi2,lzcnt,popcnt")
// #pragma GCC optimize("trapv")
