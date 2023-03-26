#include <bits/stdc++.h>
#include "Treap.h"
#include "KNN.h"
#include <random>
using namespace std;
using namespace std::chrono;

struct Solver {
    int n;
    mt19937 rng;
    vector<pair<double,double>> points;
    vector<vector<double>> dists;
    KNN knn;

    Solver() { 
        cin >> n;
        for (int i = 0; i < n; i++) {
            double u, v; cin >> u >> v;
            points.push_back({u, v});
        }
        knn = KNN(points);
        compute_distances();
        rng = mt19937(69420);
    }

    void compute_distances() {
        dists = vector<vector<double>>(n, vector<double>(n));
        for (int i = 0; i < n; i++) {
            auto [x, y] = points[i];
            for (int j = 0; j < n; j++) {
                auto [nx, ny] = points[j];
                dists[i][j] = (1.0*(x-nx)*(x-nx)) + (1.0*(y-ny)*(y-ny));
            }
        }
    }

    void solve() {
        double best = -1;
        vector<int> solution(n), permutation(n);
        auto begin = high_resolution_clock::now();
        while (true) {
            auto cur = high_resolution_clock::now();
            if (duration_cast<milliseconds>(cur - begin).count() >= 1000) {
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
        int total_visited = 0;
        vector<int> visited(n), permutation(n);
        int current = uniform_int_distribution<int>(0, n-1)(rng);
        while (total_visited < n) {
            permutation[total_visited] = current;
            visited[current] = 1;
            vector<int> nbrs = knn.find(points[current]);
            
            bool updated = false;
            double best = -1;
            for (int i = 0; i < nbrs.size(); i++) {
                if (visited[nbrs[i]]) continue;
                if (dists[current][nbrs[i]] < best || best == -1) {
                    best = dists[current][nbrs[i]];
                    current = nbrs[i];
                    updated = true;
                }
            }
            if (!updated) {
                for (int i = 0; i < n; i++) {
                    if (visited[i]) continue;
                    current = i;
                    break;
                }
            }
            total_visited++;
        }
        return permutation;
    }

    double local_search(vector<int> &p) {
        Treap tree;
        double score = 0.0;
        for (int i = 0; i < n; i++) {
            int cur = p[i];
            int next = p[(i + 1)%n];
            score += sqrt(dists[cur][next]);
            tree.ins(cur, i);
        }

        // Is this a good amount?
        for (int i = 0; i < n * 10; i++) {
            // Select a random indices within the range [0, n-1]
            int index = uniform_int_distribution<int>(0, n-1)(rng);
            int node = tree.get(index);

            // Find some close neighbors
            double best_dif = 0;
            int best_index1 = -1, best_node2 = -1;
            int best_index2 = -1, best_node1_next = -1;
            vector<int> nbrs = knn.find(points[node]);
            for (int j = 0; j < max((int)nbrs.size(), 1); j++) {
                int index1 = index, node1 = node;
                int index2 = -1, node2 = -1;
                if (nbrs.size() <= 1) {
                    index2 = uniform_int_distribution<int>(0, n-1)(rng);
                    node2 = tree.get(index2);
                } else {
                    index2 = nbrs[j];
                    node2 = tree.get(index2);
                }

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
                auto change = sqrt(dists[node1_next][node2_next]) + sqrt(dists[node1][node2]);
                auto orig = sqrt(dists[node1][node1_next]) + sqrt(dists[node2][node2_next]);
                auto dif = change - orig;
                if (dif < best_dif) {
                    best_dif = dif;
                    best_index1 = index1;
                    best_node2 = node2;
                    best_index2 = index2;
                    best_node1_next = node1_next;
                }
            }

            // If the change in score is negative, update the tree and the score
            if (best_dif >= 0) continue;
            tree.set(best_node2, best_index1 + 1);
            tree.set(best_node1_next, best_index2);
            if (best_index1 + 2 < best_index2 - 1) {
                tree.reverse(best_index1 + 2, best_index2 - 1);
            }
            score += best_dif;
        }
        // save final permutation
        for (int i = 0; i < n; i++) {
            p[i] = tree.get(i);
        }
        return score;
    }

    void test_knn() {
        int index = 1;
        printf("Looking for (%f, %f)\n", points[index].first, points[index].second);
        vector<int> ans = knn.find(points[index]);
        for (int i: ans) {
            printf("(%f, %f) -> ", points[i].first, points[i].second);
        }
        printf("\n");
    }
};

int main() {
    Solver s = Solver();
    s.solve();
    // s.test_knn();
}