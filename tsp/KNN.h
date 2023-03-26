#include <random>
#include <unordered_map>

/* an APPROXIMATE KNN algo */
struct KNN {
    typedef pair<double,double> Point;
    typedef pair<Point,Point> Line;

    vector<Point> points;
    vector<vector<Line>> lines;
    unordered_multimap<int,int> mp;
    int K = 1;
    mt19937 rng;
    KNN() {}
    KNN(vector<Point> points): points(points) {
        rng = mt19937(69420);
        build();
    }

    pair<Point, Point> get_min_max(const vector<Point>& points) {
        Point min_coords(numeric_limits<double>::max(),
                        numeric_limits<double>::max());
        Point max_coords(numeric_limits<double>::lowest(),
                        numeric_limits<double>::lowest());

        for (const auto& p : points) {
            min_coords.first = min(min_coords.first, p.first);
            min_coords.second = min(min_coords.second, p.second);
            max_coords.first = max(max_coords.first, p.first);
            max_coords.second = max(max_coords.second, p.second);
        }

        return make_pair(min_coords, max_coords);
    }

    void build() {
        lines.resize(K);
        int n = points.size();
        int nlines = 0;
        while ((1 << nlines) < n) {
            nlines++;
        }
        // nlines+=10;

        pair<Point, Point> min_max = get_min_max(points);
        auto [minX, minY] = min_max.first;
        auto [maxX, maxY] = min_max.second;
        uniform_real_distribution<double> distX(minX, maxX);
        uniform_real_distribution<double> distY(minY, maxY);
        for (int k = 0; k < K; k++) {
            for (int i = 0; i < nlines; i++) {
                lines[k].push_back({
                    {distX(rng), distY(rng)},
                    {distX(rng), distY(rng)}
                });
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
     * P - A => (x - x_1, y - y_1)
     * Dot product and check sign.
     */
    int hash(Point P, int k) {
        cout<<"("<<P.first<<", "<<P.second<<")"<<endl;
        int hsh = 0;
        for (int i = 0; i < lines[k].size(); i++) {
            Line line = lines[k][i];
            Point A = line.first;
            Point B = line.second;
            double dot = (P.first - A.first) * (B.second - A.second);
            dot -= (P.second - A.second) * (B.first - A.first);
            cout<<"DOT: "<<dot<<endl;
            hsh |= (dot >= 0) << i;

        }
        cout<<"HSH: "<<bitset<14>(hsh)<<endl;
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