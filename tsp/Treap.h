using namespace std;
struct Node {
	Node *l = 0, *r = 0;
	int val, y, c = 1;
    bool rev = false;
	Node(int val) : val(val), y(rand()) {}
	void recalc();
};

int cnt(Node* n) { return n ? n->c : 0; }
void Node::recalc() { c = cnt(l) + cnt(r) + 1; }

void push(Node* it) {
    if (it && it->rev) {
        it->rev = false;
        swap (it->l, it->r);
        if (it->l)  it->l->rev ^= true;
        if (it->r)  it->r->rev ^= true;
    }
}

pair<Node*, Node*> split(Node* n, int k) {
	if (!n) return {};
    push(n);
	if (cnt(n->l) >= k) { // "n->val >= k" for lower_bound(k)
		auto pa = split(n->l, k);
		n->l = pa.second;
		n->recalc();
		return {pa.first, n};
	} else {
		auto pa = split(n->r, k - cnt(n->l) - 1); // and just "k"
		n->r = pa.first;
		n->recalc();
		return {n, pa.second};
	}
}

Node* merge(Node* l, Node* r) {
    push(l), push(r);
	if (!l) return r;
	if (!r) return l;
	if (l->y > r->y) {
		l->r = merge(l->r, r);
		l->recalc();
		return l;
	} else {
		r->l = merge(l, r->l);
		r->recalc();
		return r;
	}
}

/* int findIndex(Node* n) {
    int count = 0;
    while (node != nullptr) {
        if (node->parent == nullptr || node == node->parent->right) {
            count += node->virtual_index + 1; // add 1 for the current node
        }
        node = node->parent;
    }
    return count - 1; 
} */

struct Treap {
    Node* tree = 0;
    Treap() {}
    void ins(int val, int pos) {
        auto pa = split(tree, pos);
        Node *node = new Node(val);
        tree = merge(merge(pa.first, node), pa.second);
    }

    /* finds index at permuation[pos] */
    int get(int pos) {
        Node *t1, *t2, *t3;
        tie(t1, t2) = split(tree, pos);
        tie(t2, t3) = split(t2, 1);
        int ans = t2->val;
        tree = merge(merge(t1, t2), t3);
        return ans;
    }

    void set(int val, int pos) {
        Node *t1, *t2, *t3;
        tie(t1, t2) = split(tree, pos);
        tie(t2, t3) = split(t2, 1);
        t2->val = val;
        tree = merge(merge(t1, t2), t3);
    }

    void reverse(int l, int r) {
        Node *t1, *t2, *t3;
        tie(t1, t2) = split(tree, l);
        tie(t2, t3) = split(t2, r-l+1);
        t2->rev ^= true;
        tree = merge(merge(t1, t2), t3);
    }
};
