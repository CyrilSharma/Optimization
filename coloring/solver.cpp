#include <bits/stdc++.h>
using namespace std;

using vi = vector<int>;
using vvi = vector<vi>;
struct Solver {
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
        solve_basic();
    }

    /* vvi get_subgraph() {

    } */

    vi solve(int cmax) {
        vi color(n), cnbrs(n);
        vvi options(n, vi(cmax));
        stack<pair<vi, vi>> states;
        bool feasible = true;
        while (true) {
            /* find best node */
            int node = -1, bestN = 1e9, bestD = -1;
            for (int i = 0; i < n; i++) {
                bool flag = false;
                if (color[i]) {
                    flag = false;
                } else if (cnbrs[i] != bestN) {
                    flag = cnbrs[i] < bestN;
                } else if (degree[i] != bestD) {
                    flag = degree[i] > bestD;
                }
                if (flag) {
                    bestN = cnbrs[i];
                    bestD = degree[i];
                    node = i;
                }
            }

            /* no nodes remaining. */
            if (node == -1) {
                feasible = false;
            }

            map<int, int> illegal;
            for (int nbr: graph[node]) {
                if (!color[nbr]) continue;
                assert(color[nbr] <= used);
                illegal[color[nbr]] = 1;
            }

            /* change */
            for (int nbr: graph[node]) {
                cnbrs[nbr]++;
            }

            bool legal = false;
            for (int c = 1; c <= min(used + 1, cmax); c++) {
                if (illegal.count(c)) continue;
                color[node] = c;
                if (c == used + 1) {
                    legal = dfs(c);
                } else {
                    legal = dfs(used);
                }
                if (legal) break;
            }

            if (legal) return true;
            /* change back */
            color[node] = 0;
            for (int nbr: graph[node]) {
                cnbrs[nbr]--;
            }
            return legal;
        }
    }

    /* 
     * on each call to dfs, teleport to the most restricted node.
     * always start HIGH. don't waste time brute forcing impossible cases.
     * use heuristic to estimate constraint. how many colored nodes are nearby.
     * yields simple increments and decrements.
     * TWO PROBLEMS
     * 1. the iteration cost is expensive? - O(E)
     * 2. without a good lower bound the entire state is brute-forced.
     */
    void solve_basic() {
        vi prev(n);
        for (int i = n; i >= 1; i--) {
            cmax = i;
            color.assign(n, 0);
            cnbrs.assign(n, i);
            bool legal = dfs(0);
            if (!legal) {
                cout<<(i + 1)<<" "<<1<<endl;
                for (int j = 0; j < n; j++) {
                    cout<<prev[j]<<" ";
                }
                return;
            }
            prev = color;
        }
    }
};

int main() {
    Solver s = Solver();
    s.solve();
}