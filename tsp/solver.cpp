#include <bits/stdc++.h>
#include "Treap.h"
#include "KDTree.h"
#include <random>
using namespace std;
using namespace std::chrono;

struct Solver {
    int n;
    mt19937 rng;
    vector<pair<double,double>> points;
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
        int total_visited = 0;
        vector<int> visited(n), permutation(n);
        int current = uniform_int_distribution<int>(0, n-1)(rng);
        set<int> legal;
        for (int i = 0; i < n; i++) {
            legal.insert(i);
        }
        while (total_visited < n) {
            permutation[total_visited] = current;
            visited[current] = 1;
            legal.erase(current);
            vector<int> nbrs = kdtree.find(points[current]);
            
            bool updated = false;
            double best = -1;
            for (int i = 0; i < nbrs.size(); i++) {
                if (visited[nbrs[i]]) continue;
                if (dist(current, nbrs[i]) < best || best == -1) {
                    best = dist(current, nbrs[i]);
                    current = nbrs[i];
                    updated = true;
                }
            }
            if (!updated) {
                current = *legal.begin();
            }
            total_visited++;
        }
        return permutation;
    }

    double local_search(vector<int> &p) {
        Treap tree;
        double score = 0.0;
        vector<double> cost(n);
        for (int i = 0; i < n; i++) {
            int cur = p[i];
            int next = p[(i + 1)%n];
            cost[cur] += dist(cur, next);
            cost[next] += dist(cur, next);
            score += dist(cur, next);
            tree.ins(cur, i);
        }

        double temp = 100;
        double cooling_rate = 0.03;
        // Is this a good amount?
        for (int i = 0; i < n * 10; i++) {
            // Select a random indices within the range [0, n-1]
            // auto [_, index] = q.top(); q.pop();//uniform_int_distribution<int>(0, n-1)(rng);
            int index = i % n;
            int node = tree.get(index);

            // Find some close neighbors
            double best_dif = 0;
            bool update = false;
            int best_index1 = -1, best_node2 = -1;
            int best_index2 = -1, best_node1_next = -1;
            vector<int> nbrs = kdtree.find(points[node]);
            // TODO: resevoir sampling? this is too many...
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
                    best_node2 = node2;
                    best_index2 = index2;
                    best_node1_next = node1_next;
                }
            }
            if (!update) continue;
            bool accept = false;
            if (best_dif < 0) accept = true;
            if (exp(-best_dif / temp) > uniform_real_distribution<double>(0,1)(rng)) {
                accept = true;
            }
            if (!accept) continue;
            tree.set(best_node2, best_index1 + 1);
            tree.set(best_node1_next, best_index2);
            if (best_index1 + 2 < best_index2 - 1) {
                tree.reverse(best_index1 + 2, best_index2 - 1);
            }
            score += best_dif;
            temp *= cooling_rate;
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