
struct point {
    double x;
    double y;
    int idx;
};
// the "hyperplane split", use comparators for all dimensions
bool cmpx(const point& a, const point& b) {return a.x < b.x;}
bool cmpy(const point& a, const point& b) {return a.y < b.y;}

struct KDTree {
	vector<point> tree;
	int n;
    KDTree() {}
	KDTree(vector<pair<double,double>> p) : n(p.size()) {
        for (int i = 0; i < n; i++) {
            tree.push_back({p[i].first, p[i].second, i});
        }
		build(0, n, false);
	}

	// k-nearest neighbor query, O(k log(k) log(n)) on average
	vector<int> find(pair<double,double> P, int k = 30) {
        auto [x, y] = P;
		perform_query(x, y, k, 0, n, false); // recurse
		vector<int> points;
		while (!pq.empty()) { // collect points
			points.push_back(pq.top().second);
			pq.pop();
		}
		reverse(points.begin(), points.end());
		return points;
	}
	// literally quicksort
	void build(int L, int R, bool dvx) {
		if (L >= R) return;
		int M = (L + R) / 2;
		// get median in O(n), split x-coordinate if dvx is true
        auto start = tree.begin();
		nth_element(start+L, start+M, start+R, dvx?cmpx:cmpy);
		build(L, M, !dvx); build(M+1, R, !dvx);
	}

	// priority queue for KNN, keep the K nearest
	priority_queue<pair<double, int> > pq;
	void perform_query(double x, double y, int k, int L, int R, bool dvx) {
		if (L >= R) return;
		int M = (L + R) / 2;
		double dx = x - tree[M].x;
		double dy = y - tree[M].y;
		double delta = dvx ? dx : dy;
		double dist = dx * dx + dy * dy;
		// if point is nearer to the kth farthest, put point in queue
		if (pq.size() < k || dist < pq.top().first) {
			pq.push(make_pair(dist, tree[M].idx));
			if (pq.size() > k) pq.pop(); // keep k elements only
		}
		int nearL = L, nearR = M, farL = M + 1, farR = R;
		if (delta > 0) { // right is nearer
			swap(nearL, farL);
			swap(nearR, farR);
		}
        // query the nearer child
		perform_query(x, y, k, nearL, nearR, !dvx);

		if (pq.size() < k || delta * delta < pq.top().first) {
            // query the farther child if there might be candidates
			perform_query(x, y, k, farL, farR, !dvx);
        }
	}
};