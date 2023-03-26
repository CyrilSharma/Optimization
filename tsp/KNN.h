#include <random>
#include <unordered_map>

/* an APPROXIMATE KNN algo */
struct KNN {
    typedef pair<double,double> Point;
    typedef pair<Point,Point> Line;

    vector<Point> points;
    vector<vector<Line>> lines;
    unordered_multimap<int,int> mp;
    int K = 3;
    mt19937 rng;
    KNN() {}
    KNN(vector<Point> points): points(points) {
        rng = mt19937(69420);
        build();
    }

    void build() {
        int n = points.size();
        int nlines = 0;
        while ((1 << nlines) < n) {
            nlines++;
        }
        /* number of sets */
        lines.resize(K);
        for (int k = 0; k < K; k++) {
            for (int i = 0; i < nlines; i++) {
                int a = 0, b = 0;
                while (a == b) {
                    a = uniform_int_distribution<int>(0, n-1)(rng);
                    b = uniform_int_distribution<int>(0, n-1)(rng);
                }
                if (a > b) swap(a, b);
                lines[k].push_back({points[a], points[b]});
            }
        }

        for (int k = 0; k < K; k++) {
            for (int i = 0; i < points.size(); i++) {
                Point P = points[i];
                mp.insert({hash(P, k), i});
            }
        }
    }

    /* normal vector is (y_2 - y_1, -(x_2 - x_1))
     * P - A => (x - x_1, y - y_2)
     * Dot product and check sign.
     */
    int hash(Point P, int k) {
        int hsh = 0;
        for (int i = 0; i < lines[k].size(); i++) {
            Line line = lines[k][i];
            Point A = line.first;
            Point B = line.second;
            double dot = (P.first - A.first) * (B.second - A.second);
            dot -= (P.second - A.second) * (B.first - A.first);
            hsh |= (dot >= 0) << i;
        }
        return hsh;
    }

    /* returns approximate knn */
    vector<int> find(Point P) {
        vector<int> ans;
        map<int,int> used;
        for (int k = 0; k < K; k++) {
            int hsh = hash(P, k);
            auto range = mp.equal_range(hsh);
            for (auto it = range.first; it != range.second; it++) {
                if (used[it->second]) continue;
                ans.push_back(it->second);
                used[it->second] = 1;
            }
        }
        return ans;
    }
};