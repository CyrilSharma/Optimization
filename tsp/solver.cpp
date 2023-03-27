#include <bits/stdc++.h>
#include "Treap.h"
#include "KDTree.h"
#include <random>
using namespace std;
using namespace std::chrono;

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
        //compute_dists();
        rng = mt19937(69420);
    }

    /* vector<vector<double, int>> dists;
    void compute_dists() {
        dists.resize(n);
        for (int i = 0; i < n; i++) {
            point a = points[i];
            vector<int> nbrs = kdtree.find(points[i]);
            for (int idx: nbrs) {
                point b = point[idx];
                double ans = sqrt((a.first-b.first)*(a.first-b.first) + 
                    (a.second-b.second)*(a.second-b.second));
                dists[i].push_back({ans, idx})
            }
        }
    } */

    double dist(int u, int v) {
        auto a = points[u];
        auto b = points[v];
        return sqrt((a.first-b.first)*(a.first-b.first) + 
        (a.second-b.second)*(a.second-b.second));
    }

    void solve() {
        double best = -1;
        vector<int> solution(n), permutation(n);
        auto begin = high_resolution_clock::now();
        while (true) {
            auto cur = high_resolution_clock::now();
            if (duration_cast<milliseconds>(cur - begin).count() >= 5000) {
                break;
            }
            /* for (int i = 0; i < n; i++) {
                permutation[i] = i;
            }
            shuffle(permutation.begin(), permutation.end(), rng); */
            permutation = greedy();
            auto score = local_search(permutation);
            if (score < best || best == -1) {
                best = score;
                solution = permutation;
            }
        }
        printf("%f %d\n", best, 0);
        for (int i = 0; i < n; i++) {
            cout<<solution[i]<<" ";
        }
        cout<<endl;
    }

    vector<int> greedy() {
        vector<int> visited(n), permutation(n);
        int current = uniform_int_distribution<int>(0, n-1)(rng);
        set<int> legal;
        for (int i = 0; i < n; i++) {
            legal.insert(i);
        }
        for (int nvisit = 0; nvisit < n; nvisit++) {
            permutation[nvisit] = current;
            visited[current] = 1;
            legal.erase(current);            
            bool updated = false;
            vector<int> nbrs = kdtree.find(points[current]);
            for (int i = 0; i < nbrs.size(); i++) {
                if (visited[nbrs[i]]) continue;
                current = nbrs[i];
                updated = true;
                break;
            }
            if (!updated) {
                current = *legal.begin();
            }
        }
        return permutation;
    }

    double local_search(vector<int> &p) {
        Treap tree;
        queue<int> q;
        set<int> unqueued;
        double score = 0.0;
        for (int i = 0; i < n; i++) {
            int cur = p[i];
            int next = p[(i + 1)%n];
            score += dist(cur, next);
            tree.ins(cur, i);
            q.push(i);
            unqueued.insert(i);
        }

        bool improving = true;
        double temp = 100000.0;
        double cooling_rate = 0.003;
        vector<int> queued(n);
        while (improving) {
            for (int i = 0; i < n; i++) {
                int index = i; // q.front(); q.pop();
                int node = tree.get(index);
                // unqueued.insert(index);
                queued[index] = 0;

                // Find some close neighbors
                double best_dif = 1e9;
                bool update = false;
                int best_index1 = -1, best_index2 = -1;
                vector<int> nbrs = kdtree.find(points[node]);
                for (int j = 0; j < max((int)nbrs.size(), 1); j++) {
                    int index1 = index, node1 = node;
                    int index2 = nbrs[j];
                    int node2 = tree.get(index2);

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

                    // Calculate the change in score if we were to swap the positions of the two nodes
                    auto change = dist(node1_next, node2_next) + dist(node1, node2);
                    auto orig = dist(node1, node1_next) + dist(node2, node2_next);
                    auto dif = change - orig;

                    if (dif < best_dif) {
                        update = true;
                        best_dif = dif;
                        best_index1 = index1;
                        best_index2 = index2;
                    }
                }
                if (best_dif >= 0) continue;
                tree.reverse(best_index1 + 1, best_index2);
                improving = true;
                last_iteration = i;
                // Your code to time here
                /* for (auto it = unqueued.lower_bound(best_index1); it != unqueued.upper_bound(best_index2); ) {
                    q.push(*it);
                    it = unqueued.erase(it);
                } */
                /* push the NODE which may be suboptimal */
                /* for (int i = best_index1; i <= best_index2; i++) {
                    // if (queued[tree.get(i)]) continue;
                    if (queued[i]) continue;
                    q.push(i);
                    //queued[tree.get(i)];
                } */
                // std::cout << "Execution time: " << duration_ms.count() << " ms" << std::endl;
                score += best_dif;
            }
        }
        // save final permutation
        for (int i = 0; i < n; i++) {
            p[i] = tree.get(i);
        }
        return score;
    }
};

int main() {
    Solver s = Solver();
    s.solve();
}