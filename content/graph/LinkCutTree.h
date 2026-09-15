/**
 * Author: Simon Lindholm
 * Date: 2016-07-25
 * Source: https://github.com/ngthanhtrung23/ACM_Notebook_new/blob/master/DataStructure/LinkCutTree.h
 * Description: Forest of unrooted trees. Link/cut, connectivity,
 *  component size and max value. Path aggregates fold splay children
 *  in \texttt{fix()}. Component aggregates also fold virtual children
 *  (\texttt{vsz}/\texttt{vmx}) when preferred children move. Size
 *  uses $+/-$; max uses a multiset. Query \texttt{compSize}/
 *  \texttt{compMax}, not \texttt{node[i].sz}.
 * Time: Amortized $O(\log N)$; $O(\log^2 N)$ with max.
 * Status: stress-tested
 * Usage: LinkCut lc(N); lc.link(0,1); lc.set(0,3); lc.compMax(0);
 */
#pragma once

struct Node { // Splay tree. Root's pp contains tree's parent.
	Node *p = 0, *pp = 0, *c[2];
	bool flip = 0;
	int val = 0, mx = 0, sz = 1, vsz = 0;
	multiset<int> vmx;
	Node() { c[0] = c[1] = 0; fix(); }
	void addVirt(Node* x) { // x becomes a virtual child
		if (!x) return;
		vsz += x->sz; vmx.insert(x->mx);
	}
	void remVirt(Node* x) {
		if (!x) return;
		vsz -= x->sz; vmx.erase(vmx.find(x->mx));
	}
	void fix() {
		if (c[0]) c[0]->p = this;
		if (c[1]) c[1]->p = this;
		sz = 1 + vsz;
		mx = val;
		if (c[0]) sz += c[0]->sz, mx = max(mx, c[0]->mx);
		if (c[1]) sz += c[1]->sz, mx = max(mx, c[1]->mx);
		if (!vmx.empty()) mx = max(mx, *vmx.rbegin());
	}
	void pushFlip() {
		if (!flip) return;
		flip = 0; swap(c[0], c[1]);
		if (c[0]) c[0]->flip ^= 1;
		if (c[1]) c[1]->flip ^= 1;
	}
	int up() { return p ? p->c[1] == this : -1; }
	void rot(int i, int b) {
		int h = i ^ b;
		Node *x = c[i], *y = b == 2 ? x : x->c[h], *z = b ? y : x;
		if ((y->p = p)) p->c[up()] = y;
		c[i] = z->c[i ^ 1];
		if (b < 2) {
			x->c[h] = y->c[h ^ 1];
			y->c[h ^ 1] = x;
		}
		z->c[i ^ 1] = this;
		fix(); x->fix(); y->fix();
		if (p) p->fix();
		swap(pp, y->pp);
	}
	void splay() { /// Splay this up to the root. Always finishes without flip set.
		for (pushFlip(); p; ) {
			if (p->p) p->p->pushFlip();
			p->pushFlip(); pushFlip();
			int c1 = up(), c2 = p->up();
			if (c2 == -1) p->rot(c1, 2);
			else p->p->rot(c2, c1 != c2);
		}
	}
	Node* first() { /// Return the min element of the subtree rooted at this, splayed to the top.
		pushFlip();
		return c[0] ? c[0]->first() : (splay(), this);
	}
};

struct LinkCut {
	vector<Node> node;
	LinkCut(int N) : node(N) {}

	void link(int u, int v) { // add an edge (u, v)
		assert(!connected(u, v));
		makeRoot(&node[u]);
		node[v].splay();
		node[v].addVirt(&node[u]);
		node[u].pp = &node[v];
		node[v].fix();
	}
	void cut(int u, int v) { // remove an edge (u, v)
		Node *x = &node[u], *top = &node[v];
		makeRoot(top); x->splay();
		assert(top == (x->pp ?: x->c[0]));
		if (x->pp) {
			x->pp->remVirt(x);
			x->pp->fix();
			x->pp = 0;
		} else {
			x->c[0] = top->p = 0;
			x->fix();
		}
	}
	bool connected(int u, int v) { // are u, v in the same tree?
		Node* nu = access(&node[u])->first();
		return nu == access(&node[v])->first();
	}
	void set(int u, int v) { // set node value
		access(&node[u]); node[u].splay();
		node[u].val = v; node[u].fix();
	}
	int compSize(int u) { return access(&node[u])->sz; }
	int compMax(int u) { return access(&node[u])->mx; }
	void makeRoot(Node* u) { /// Move u to root of represented tree.
		access(u);
		u->splay();
		if(u->c[0]) {
			u->c[0]->p = 0;
			u->c[0]->flip ^= 1;
			u->c[0]->pp = u;
			u->addVirt(u->c[0]);
			u->c[0] = 0;
			u->fix();
		}
	}
	Node* access(Node* u) { /// Move u to root aux tree. Return the root of the root aux tree.
		u->splay();
		while (Node* pp = u->pp) {
			pp->splay(); u->pp = 0;
			if (Node* r = pp->c[1]) {
				r->p = 0; r->pp = pp; pp->addVirt(r); }
			pp->remVirt(u); pp->c[1] = u;
			pp->fix(); u = pp;
		}
		return u;
	}
};
