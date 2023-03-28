#include <bits/stdc++.h>
#include "Treap.h"
#include "KDTree.h"
#include <random>
using namespace std;
using namespace std::chrono;

struct DSU {
    typedef vector<int> vi;
    int nc; vi parent, size;
    DSU(int n) {parent=vi(n,-1); size=vi(n,1); nc=n;}
    int find(int v) {
        if (parent[v] == -1) return v;
        return parent[v] = find(parent[v]);
    }
    void merge(int a, int b) {
        int r1 = find(a); int r2 = find(b);
        if (r1 == r2) return;
        if (size[r2] > size[r1]) swap(r1,r2);
        parent[r2] = r1; size[r1] += size[r2];
        nc--;
    }
};

struct Solver {
    typedef pair<double,double> point;

    int n;
    mt19937 rng;
    vector<point> points;
    KDTree kdtree;

    Solver() { 
        cin >> n;
        for (int i = 0; i < n; i++) {
            double u, v; cin >> u >> v;
            points.push_back({u, v});
        }
        kdtree = KDTree(points);
        rng = mt19937(69420);
    }

    double dist(int u, int v) {
        auto a = points[u];
        auto b = points[v];
        return sqrt((a.first-b.first)*(a.first-b.first) + 
        (a.second-b.second)*(a.second-b.second));
    }

    void solve() {
        double best = -1, score = -1;
        vector<int> solution(n), permutation(n);
        auto begin = high_resolution_clock::now();
        permutation = greedy();
        while (true) {
            auto cur = high_resolution_clock::now();
            if (duration_cast<milliseconds>(cur - begin).count() >= 50000) {
                break;
            }
            /* for (int i = 0; i < n; i++) {
                permutation[i] = i;
            }
            shuffle(permutation.begin(), permutation.end(), rng); */
            auto start = permutation;
            score = simulatedAnnealing(start);
            //permutation = greedy();
            
            if (score < best || best == -1) {
                best = score;
                solution = start;
            }
        }
        //cout<<"BEFORE: "<<best<<endl;
        best = greedy2Opt(solution);
        //cout<<"AFTER: "<<best<<endl;
        printf("%f %d\n", best, 0);
        for (int i = 0; i < n; i++) {
            cout<<solution[i]<<" ";
        }
        cout<<endl;
    }

    vector<int> greedy() {
        vector<pair<int,int>> edges;
        for (int i = 0; i < n; i++) {
            vector<int> nbrs = kdtree.find(points[i]);
            for (int j : nbrs) {
                if (i == j) continue;
                edges.push_back({i, j});
                edges.push_back({j, i});
            }
        }
        sort(edges.begin(), edges.end(), [&](pair<int,int> a, pair<int,int> b) {
            return dist(a.first, a.second) < dist(b.first, b.second);
        });

        set<int> start, end;
        for (int i = 0; i < n; i++) {
            start.insert(i);
            end.insert(i);
        }

        DSU dsu(n);
        vector<int> next(n, -1);
        double len = 0.0;
        for (int i = 0; i < edges.size(); i++) {
            if (dsu.size[0] == n) break;
            auto [u, v] = edges[i];
            if (!start.count(u)) continue;
            if (!end.count(v)) continue;
            if (dsu.find(u) == dsu.find(v)) continue;
            len += dist(u, v);
            next[u] = v;
            dsu.merge(u, v);
            start.erase(u);
            end.erase(v);
        }
        assert(start.size() == end.size());
        for (auto it = start.begin(); it != start.end(); ) {
            auto u = *it;
            bool found = false;
            for (auto v: end) {
                if (dsu.find(u) == dsu.find(v)) continue;
                if (u == v) continue;
                len += dist(u, v);
                next[u] = v;
                dsu.merge(u, v);
                end.erase(v);
                found = true;
                break;
            }
            if (found) {
                it = start.erase(it);
            } else {
                it++;
            }
        }

        next[*start.begin()] = *end.begin();
        len += dist(*start.begin(), *end.begin());

        vector<int> permutation(n);
        int cur = 0;
        for (int count = 0; count < n; count++) {
            permutation[count] = cur;
            cur = next[cur];
        }
        for (int i = 0; i < n; i++) {
            assert(permutation[i] != -1);
        }
        return permutation;
    }

    double simulatedAnnealing(vector<int> &p) {
        //cout<<"NEW RUN"<<endl;
        Treap tree(p);
        double score = 0.0;
        vector<double> costs(n);
        for (int i = 0; i < n; i++) {
            int cur = p[i];
            int next = p[(i + 1)%n];
            costs[cur] += dist(cur, next);
            costs[next] += dist(cur, next);
            score += dist(cur, next);
        }

        priority_queue<pair<double, int>> q;
        for (int i = 0; i < n; i++) {
            q.push({costs[p[i]], p[i]});
        }

        double temp = 2.0;
        double cooling_rate = 0.03;
        auto accept = [&](double delta) {
            if (delta < 0) return true;
            double expo = exp(-delta/temp);
            double rndm = uniform_real_distribution<double>(0, 1)(rng);
            return expo > rndm;
        };

        bool improving = true;
        int iters = 0;
        while (improving) {
            int accepts = 0;
            int total = 0;
            improving = false;
            /* iterate over the 20 most egregious costs */
            double cscore = score;
            for (int i = 0; i < max(n/100, 20); i++) {
                /* dequeue old elements */
                int node = -1;
                while (!q.empty()) {
                    auto [c, idx] = q.top(); q.pop();
                    if (c != costs[idx]) continue;
                    node = idx;
                    break;
                }
                if (node == -1) break;
                vector<int> nbrs = kdtree.find(points[node]);
                for (int iters = 0; iters < 10; iters++) {
                    int j = uniform_int_distribution<int>(0, nbrs.size()-1)(rng);
                    //int j = iters;
                    int index1 = tree.find(node);
                    int index2 = tree.find(nbrs[j]);
                    int node1 = node;
                    int node2 = nbrs[j];
                    assert(tree.get(index1) == node1);
                    assert(tree.get(index2) == node2);
                    if (index1 > index2) {
                        swap(index1, index2);
                        swap(node1, node2);
                    }
                    assert(tree.get(index1) == node1);
                    assert(tree.get(index2) == node2);
                    if ((n + index2 - index1) % n <= 1) continue;
                    int node1_next = tree.get((index1 + 1) % n);
                    int node2_next = tree.get((index2 + 1) % n);
                    double change = dist(node1_next, node2_next) + dist(node1, node2);
                    double orig = dist(node1, node1_next) + dist(node2, node2_next);
                    double delta = change - orig;
                    if (!accept(delta)) continue;
                    if (score < cscore) improving = true;
                    score += delta;
                    tree.reverse(index1 + 1, index2);


                    /* update costs */
                    vector<int> nodes = {node1, node2, node1_next, node2_next};
                    for (int item: nodes) {
                        int node_idx = tree.find(item);
                        int prev = (n + node_idx - 1) % n;
                        int next = (n + node_idx + 1) % n;
                        double ndist = dist(tree.get(prev), item);
                        ndist += dist(item, tree.get(next));
                        costs[item] = ndist;
                        q.push({costs[item], item});
                    }
                }
            }
            /* why is this negative wtaf */
            //cout<<"SCORE: "<<score<<endl;
            //printf("RATIO: %.2f\n", (1.0 * accepts / total));
            temp *= (1 - cooling_rate);
            iters++;
        }

        // save final permutation
        for (int i = 0; i < n; i++) {
            p[i] = tree.get(i);
        }
        return score;
    }

    double greedy2Opt(vector<int> &p) {
        Treap tree(p);
        double score = 0.0;
        for (int i = 0; i < n; i++) {
            int cur = p[i];
            int next = p[(i + 1)%n];
            score += dist(cur, next);
        }

        int iters = 0;
        bool improving = true;
        vector<int> queued(n);
        while (improving) {
            improving = false;
            for (int i = 0; i < n; i++) {
                int index = i;
                int node = tree.get(index);

                // Find some close neighbors
                double best_dif = 1e9;
                int best_index1 = -1, best_index2 = -1;
                vector<int> nbrs = kdtree.find(points[node]);
                for (int j = 0; j < max((int)nbrs.size(), 1); j++) {
                    int index1 = index, node1 = node;
                    int index2 = tree.find(nbrs[j]);
                    int node2 = nbrs[j];
                    assert(node2 == tree.get(index2));

                    // Ensure that index1 < index2
                    if (index1 > index2) {
                        swap(index1, index2);
                        swap(node1, node2);
                    }

                    // If the difference between the two indices is less than or equal to 1, skip this iteration
                    if ((n + index2 - index1) % n <= 1) continue;

                    // Find the next nodes.
                    int node1_next = tree.get((index1 + 1) % n);
                    int node2_next = tree.get((index2 + 1) % n);

                    // Calculate the change in score if we were to swap the positions of the two
                    auto change = dist(node1_next, node2_next) + dist(node1, node2);
                    auto orig = dist(node1, node1_next) + dist(node2, node2_next);
                    auto dif = change - orig;

                    if (dif < best_dif) {
                        best_dif = dif;
                        best_index1 = index1;
                        best_index2 = index2;
                    }
                }
                if (best_dif >= 0) continue;
                tree.reverse(best_index1 + 1, best_index2);
                improving = true;
                score += best_dif;
                iters++;
            }
        }
        //cout<<"ITERS: "<<iters<<endl;
        // save final permutation
        for (int i = 0; i < n; i++) {
            p[i] = tree.get(i);
        }
        return score;
    }

    void test() {
        n = 50;
        vector<int> ordered(n);
        for (int i = 0; i < n; i++) {
            ordered[i] = i;
        }
        auto reverse = [&](int a, int b) {
            while (a < b) {
                int tmp = ordered[b];
                ordered[b] = ordered[a];
                ordered[a] = tmp;
                a++, b--;
            }
        };
        Treap tree(ordered);

        for (int i = 0; i < 1e6; i++) {
            int a = uniform_int_distribution<int>(0, n-1)(rng);
            int b = uniform_int_distribution<int>(0, n-1)(rng);
            int c = uniform_int_distribution<int>(0, n-1)(rng);
            int d = uniform_int_distribution<int>(0, n-1)(rng);
            if (a > b) swap(a, b);
            tree.reverse(a, b);
            tree.get(c);
            tree.find(d);
            reverse(a, b);
        }

        vector<int> position(n);
        for (int i = 0; i < n; i++) {
            position[ordered[i]] = i;
        }

        for (int i = 0; i < n; i++) {
            assert(tree.get(i) == ordered[i]);
            assert(tree.find(i) == position[i]);
        }
    }
};

int main() {
    Solver s = Solver();
    s.solve();
}