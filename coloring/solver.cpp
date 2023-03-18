#include <bits/stdc++.h>
using namespace std;
using namespace std::chrono;

using vi = vector<int>;
using vvi = vector<vi>;
struct Solver {
    time_point<high_resolution_clock> begin;
    int n, e;
    vi degree;
    vvi graph;

    Solver() { 
        cin >> n >> e; 
        degree.resize(n);
        graph.resize(n);
        for (int i = 0; i < e; i++) {
            int a, b;
            cin >> a >> b;
            graph[a].push_back(b);
            graph[b].push_back(a);
            degree[a]++;
            degree[b]++;
        }
    }

    void solve() {
        begin = high_resolution_clock::now();
        int max_degree = *max_element(degree.begin(), degree.end());
        int prev_best = 1e9;
        vi prev(n), color(n);
        for (int i = min(max_degree + 1, n); i >= 1; i--) {
            int status = solve(color, i);
            if (status < 0) {
                cout<<(prev_best)<<" "<<(status == -1)<<endl;
                for (int j = 0; j < n; j++) {
                    cout<<(prev[j] - 1)<<" ";
                }
                return;
            }
            prev = color;
            prev_best = status;
            color.assign(n, 0);
            i = status;
        }
    }

    struct delta {
        vector<int> del_color;
        vector<pair<int,int>> del_legal;
        int node, start, used;
    };

    /* We're not updating the state wrong - 
     * all of our results were feasible,
     * yet we wrongly concloud they are optimal...
     */
    int solve(vi &color, int cmax) {
        int start = 1, node = -1, used = 0, prev_used = 0;
        bool should_compute = true;
        vector<set<int>> legal(n);
        for (int i = 0; i < n; i++) {
            for (int c = 1; c <= cmax; c++) {
                legal[i].insert(c);
            }
        }
        vector<pair<int,int>> del_legal;
        vector<int> del_color;
        stack<delta> deltas;

        auto find_best = [&]() {
            int ind = -1, bestD = -1, bestL = 1e9;
            for (int i = 0; i < n; i++) {
                if (color[i]) continue;
                bool flag = false;
                if ((int) legal[i].size() != bestL) {
                    flag = (int) legal[i].size() < bestL;
                } else if (degree[i] != bestD) {
                    flag = degree[i] > bestD;
                }
                if (flag) {
                    bestL = legal[i].size();
                    bestD = degree[i];
                    ind = i;
                }
            }
            return ind;
        };

        auto backtrack = [&]() {
            delta d = deltas.top();
            for (auto ind: d.del_color) {
                assert(color[ind]);
                color[ind] = 0;
            }

            for (auto [nbr, c]: d.del_legal) {
                assert(!legal[nbr].count(c));
                legal[nbr].insert(c);
            }

            node = d.node;
            start = d.start;
            used = d.used;
            should_compute = false;
            deltas.pop();
        };

        while (true) {
            /* check for timeout. */
            auto cur = high_resolution_clock::now(); 
            if (duration_cast<milliseconds>(cur - begin).count() >= 5000) {
                return -2;
            }
            if (should_compute) {
                node = find_best();
                if (node == -1) {
                    return used;
                }
            }

            /* choose a color */
            auto ptr = legal[node].lower_bound(start);
            if (ptr == legal[node].end()) {
                if (deltas.empty()) break;
                backtrack();
                continue;
            }
            int chosen = *ptr; 
            if (chosen == used + 1) {
                used++;
            }
            
            prev_used = used;
            del_color.resize(0);
            del_legal.resize(0);

            /* branching effects */
            for (int nbr: graph[node]) {
                if (!legal[nbr].count(chosen)) continue;
                legal[nbr].erase(chosen);
                del_legal.push_back({nbr, chosen});
            }
            color[node] = chosen;
            del_color.push_back(node);

            /* propogation */
            vi visited(n);
            queue<int> q;
            q.push(node);
            bool feasible = true;
            while (feasible && !q.empty()) {
                int cur = q.front(); q.pop();
                if (color[cur]) continue;
                if ((int) legal[cur].size() == 0) {
                    feasible = false;
                } else if ((int) legal[cur].size() == 1) {
                    color[cur] = *legal[cur].begin();
                    if (color[cur] == used + 1) {
                        used++;
                    }
                    del_color.push_back(cur);
                    for (int nbr: graph[cur]) {
                        if (color[nbr] &&
                            color[nbr] == color[cur]) {
                            feasible = false;
                            break;
                        }
                        if (color[nbr]) continue;
                        if (!legal[nbr].count(color[cur])) continue;
                        legal[nbr].erase(color[cur]);
                        del_legal.push_back({nbr, color[cur]});
                        if ((int) legal[nbr].size() == 1) {
                            if (visited[nbr]) continue;
                            visited[nbr] = 1;
                            q.push(nbr);
                        } else if ((int) legal[nbr].size() == 0) {
                            feasible = false;
                            break;
                        }
                    }
                }
            }

            delta d = {del_color, del_legal, node, chosen + 1, prev_used};
            deltas.push(d);
            if (feasible) {
                start = 1;
                should_compute = true;
                continue;
            }

            if (deltas.empty()) break;
            backtrack();
        }
        return -1;
    }
};

int main() {
    Solver s = Solver();
    s.solve();
}
/* This can be improved potentially a lot if the all-different constraint
 * and clique detection is implemented.
 */