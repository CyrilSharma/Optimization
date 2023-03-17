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
        int max_degree = -1;
        for (int i = 0; i < n; i++) {
            max_degree = max(max_degree, degree[i]);
        }
        vi prev(n), color(n);
        for (int i = min(max_degree + 1, n); i >= 1; i--) {
            bool legal = solve(color, i);
            if (!legal) {
                cout<<(i + 1)<<" "<<1<<endl;
                for (int j = 0; j < n; j++) {
                    cout<<(prev[j] - 1)<<" ";
                }
                return;
            }
            prev = color;
            color.assign(n, 0);
        }
    }

    struct state {
        vi &color;
        vector<map<int,int>> &illegal;
        int node;
        int start;
        int used;
    };

    int solve(vi &color, int cmax) {
        // cout<<"CMAX: "<<cmax<<endl;
        int start = 1, used = 0;
        vector<map<int, int>> illegal(n);
        stack<state> states;
        int node = -1;
        while (true) {
            /* find best node */
            if (node == -1) {
                int bestD = -1, bestIL = -1;
                for (int i = 0; i < n; i++) {
                    bool flag = false;
                    if (color[i]) {
                        flag = false;
                    } else if ((int) illegal[i].size() != bestIL) {
                        flag = (int) illegal[i].size() > bestIL;
                    } else if (degree[i] != bestD) {
                        flag = degree[i] > bestD;
                    }
                    if (flag) {
                        bestIL = illegal.size();
                        bestD = degree[i];
                        node = i;
                    }
                }
                /* no nodes remaining. solution is feasible */
                if (node == -1) {
                    return 1;
                }
            }

            /* choose a color */
            bool feasible = false;
            for (int c = start; c <= min(used + 1, cmax); c++) {
                if (illegal[node].count(c)) continue;
                feasible = true;
                color[node] = c;
                state s = {color, illegal, node, c + 1, used};
                states.push(s);
                if (c == used + 1) {
                    used++;
                }
                break;
            }

            /* add branching effects */
            for (int nbr: graph[node]) {
                if (color[nbr] > 0 &&
                    color[nbr] == color[node]) {
                    feasible = false;
                    break;
                }
                illegal[nbr][color[node]] = 1;
            }

            /* update state */
            while (feasible) {
                bool updated = false;
                for (int i = 0; i < n; i++) {
                    if (color[i]) continue;
                    if ((int) illegal[i].size() == cmax) {
                        feasible = false;
                        break;
                      /* 1 option left */
                    } else if ((int) illegal[i].size() == cmax - 1) {
                        for (int c = 1; c <= min(used + 1, cmax); c++) {
                            if (illegal[i].count(c)) continue;
                            color[i] = c;
                            if (c == used + 1) {
                                used++;
                            }
                            break; 
                        }
                        for (int nbr: graph[i]) {
                            if (color[nbr] > 0 &&
                                color[nbr] == color[i]) {
                                feasible = false;
                                break;
                            }
                            illegal[nbr][color[i]] = 1;
                        }
                        updated = true;
                    }
                }
                if (!updated) break;
            }
            if (feasible) {
                node = -1;
                start = 1;
                continue;
            }

            /* backtrack if infeasible */
            if (!states.empty()) {
                state s = states.top();
                states.pop();

                color = s.color;
                illegal = s.illegal;
                node = s.node;
                start = s.start;
                used = s.used;
            } else {
                break;
            }
        }
        return 0;
    }
};

int main() {
    Solver s = Solver();
    s.solve();
}