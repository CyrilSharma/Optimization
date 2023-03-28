using namespace std;
struct Node {
	Node *l = 0, *r = 0, *p = 0;
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
        if (pa.first) pa.first->p = 0;
        if (pa.second) pa.second->p = n;
		return {pa.first, n};
	} else {
		auto pa = split(n->r, k - cnt(n->l) - 1); // and just "k"
		n->r = pa.first;
		n->recalc();
        if (pa.first) pa.first->p = n;
        if (pa.second) pa.second->p = 0;
		return {n, pa.second};
	}
}

Node* merge(Node* l, Node* r) {
    push(l), push(r);
	if (!l) return r;
	if (!r) return l;
	if (l->y > r->y) {
		l->r = merge(l->r, r);
        l->r->p = l;
		l->recalc();
		return l;
	} else {
		r->l = merge(l, r->l);
        r->l->p = r;
		r->recalc();
		return r;
	}
}

struct Treap {
    int n;
    Node* tree = 0;
    vector<Node*> nodes;
    Treap(vector<int> p) : n(p.size())  {
        nodes.resize(n);
        for (int i = 0; i < n; i++) {
            ins(p[i], i);
        }
    }
    void ins(int val, int pos) {
        auto pa = split(tree, pos);
        Node *node = new Node(val);
        tree = merge(merge(pa.first, node), pa.second);
        nodes[val] = node;
    }

    /* gets value at permuation[pos] */
    int get(int pos) {
        Node *t1, *t2, *t3;
        tie(t1, t2) = split(tree, pos);
        tie(t2, t3) = split(t2, 1);
        int ans = t2->val;
        tree = merge(merge(t1, t2), t3);
        return ans;
    }

    void print() {
        queue<pair<int,Node *>> q;
        q.push({0, tree});
        int clevel = -1;
        while (!q.empty()) {
            auto [level, node] = q.front(); q.pop();
            if (level > clevel) {
                clevel = level;
                cout<<endl;
            }
            cout<<(node ? node->val : -1);
            cout<<"--"<<(node ? node->c : -1);
            for (int i = 0; i < 4; i++) {
                cout<<" ";
            }
            if (node) {
                q.push({level + 1, node->l});
                q.push({level + 1, node->r});
            }
        }
    }

    /* finds index S.T permutation[pos] = val */
    int find(int val) {
        Node* prev = NULL;
        Node* cur = nodes[val];
        int count = 0;
        while (cur != NULL) {
            if (prev == NULL || (cur->r && cur->r->val == prev->val)) {
                count += cnt(cur->l) + 1;
            }
            if (cur->rev) count = cur->c - count + 1;
            prev = cur;
            cur = cur->p;
        }
        return count - 1;
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
