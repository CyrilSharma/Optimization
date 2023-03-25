#include <bits/stdc++.h>
#include <random>
using namespace std;
using namespace std::chrono;

struct Solver {
    int n;
    mt19937 rng;
    vector<pair<double,double>> points;
    vector<vector<double>> dists;

    Solver() { 
        cin >> n;
        for (int i = 0; i < n; i++) {
            double u, v; cin >> u >> v;
            points.push_back({u, v});
        }
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
            if (duration_cast<milliseconds>(cur - begin).count() >= 5000) {
                break;
            }
            for (int i = 0; i < n; i++) {
                permutation[i] = i;
            }
            shuffle(permutation.begin(), permutation.end(), rng);
            auto score = local_search(permutation);
            if (score > best) {
                best = score;
                solution = permutation;
            }
            best = max(best, score);
        }
        cout<<best<<" "<<0<<endl;
        for (int i = 0; i < n; i++) {
            cout<<solution[i]<<" ";
        }
        cout<<endl;
    }

    double local_search(vector<int> &permutation) {
        // int uniform_int_distribution<int>(0, n)(rng);
        double score = -1;
        vector<int> point(n);
        for (int i = 0; i < n - 1; i++) {
            int cur = permutation[i];
            int next = permutation[i + 1];
            point[cur] = next;
            score += dists[cur][next];
        }

        point[n - 1] = permutation[0];
        score += dists[n - 1][0];
        int u, v, u_next, v_next;
        for (int i = 0; i < max(100, n/100); i++) {
            u = uniform_int_distribution<int>(0, n-1)(rng);
            v = uniform_int_distribution<int>(0, n-1)(rng);
            if (u == v) continue;
            u_next = point[u];
            v_next = point[v];
            auto change = (dists[u_next][v_next] + dists[u][v]);
            auto orig = (dists[u][u_next] + dists[v][v_next]);
            auto dif = change - orig;
            if (dif < 0) {
                point[u_next] = v_next;
                point[v] = u;
                score += dif;
            }
        }

        permutation[0] = point[n - 1];
        for (int i = 1; i < n; i++) {
            permutation[i] = point[i - 1];
        }
        return score;
    }
};

int main() {
    Solver s = Solver();
    s.solve();
}