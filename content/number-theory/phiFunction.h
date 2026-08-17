/**
 * Author: Håkan Terelius
 * Date: 2009-09-25
 * License: CC0
 * Source: http://en.wikipedia.org/wiki/Euler's_totient_function
 * Description: \emph{Euler's $\phi$} function is the number of integers in $[1,n]$ coprime to $n$. $\phi(1)=1$.\\
 * $p$ prime $\Rightarrow \phi(p^k)=(p-1)p^{k-1}$.
 * $m,n$ coprime $\Rightarrow \phi(mn)=\phi(m)\phi(n)$.\\
 * If $n=\prod_i p_i^{k_i}$ then $\phi(n)=\prod_i (p_i-1)p_i^{k_i-1}=n\prod_{p\mid n}(1-1/p)$.\\
 * $\sum_{d\mid n}\phi(d)=n$. For $n>1$, $\sum_{k\bot n}k=n\phi(n)/2$.\\
 * \textbf{Euler's thm:} $a,n$ coprime $\Rightarrow a^{\phi(n)}\equiv 1\pmod n$.\\
 * \textbf{Fermat's little thm:} $p$ prime $\Rightarrow a^{p-1}\equiv 1\pmod p$ $\forall a$.
 * Status: Tested
 */
#pragma once

const int LIM = 5000000;
int phi[LIM];

void calculatePhi() {
	rep(i,0,LIM) phi[i] = i&1 ? i : i/2;
	for (int i = 3; i < LIM; i += 2) if(phi[i] == i)
		for (int j = i; j < LIM; j += i) phi[j] -= phi[j] / i;
}
