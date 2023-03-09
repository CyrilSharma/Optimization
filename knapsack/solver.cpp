#include <bits/stdc++.h>
using namespace std;

using ll = long long;
using vi = vector<int>;
using vvi = vector<vi>;
using vll = vector<ll>;
using trip = tuple<int,int,int>;

struct Solver {
    int n, k, max_iters;
    vi vals, wts;
    Solver(int iters) { 
        max_iters = iters;
        cin >> n >> k; 
        vals.resize(n);
        wts.resize(n);
        for (int i = 0; i < n; i++) {
            int v, w; cin >> v >> w;
            vals[i] = v;
            wts[i] = w;
        }
    }

    void solve() {
        if (n * k < 1e9) {
            dp_solve();
        } else {
            lds_solve();
        }
    }

    void dp_solve() {
        vvi dp(n+1, vi(k+1));
        for (int i = 1; i <= n; i++) {
            int v = vals[i-1], w = wts[i-1];
            for (int j = 0; j <= k; j++) {
                if (j < w) {
                    dp[i][j] = dp[i-1][j];
                    continue;
                }
                dp[i][j] = max(
                    dp[i-1][j-w] + v,
                    dp[i-1][j]
                );
            }
        }

        int index = k;
        for (int i = k; i >= 0; i--) {
            if (dp[n][i] <= 0) continue;
            cout<<dp[n][i]<<" "<<1<<endl;
            index = i;
            break;
        }

        vi ans(n+1);
        int cur = k, val = dp[n][index];
        for (int i = n; i >= 1; i--) {
            if (dp[i - 1][cur] == val) continue;
            ans[i] = 1;
            cur -= wts[i - 1];
            val -= vals[i - 1];
        }

        for (int i = 1; i <= n; i++) {
            cout<<ans[i]<<" ";
        }
    }

    void lds_solve() {
        vector<trip> items(n);
        for (int i = 0; i < n; i++) {
            int v = vals[i], w = wts[i];
            items[i] = {v, w, i};
        }

        /* sort by value density */
        vector<trip> temp = items;
        sort(temp.begin(), temp.end(), [&](trip a, trip b){
            auto [av, aw, ai] = a;
            auto [bv, bw, bi] = b;
            return (av * 1.0 / aw) > 
                (bv * 1.0 / bw);
        });

        /* compute cumulatives and heuristics */
        vi used(n);
        vll cvalue(n + 1);
        vll cspace(n + 1);
        ll mn = 0;
        int space = 0;
        for (int i = 1; i <= n; i++) {
            auto [v, w, ind] = temp[i-1];
            cvalue[i] = cvalue[i-1] + v;
            cspace[i] = cspace[i-1] + w;
            if (space > k) continue;
            used[ind] = 1;
            if (space + w < k) {
                mn += v;
            }
            space += w;
        }

        /* binary search on the estimate */
        auto estimate = [&](int score, ll room, int start) {
            if (room < 0) return -1.0;
            /* find 0-index [start, l] valid */
            int l = start - 1, r = n - 1;
            while (l < r) {
                int m = (l + r + 1) / 2;
                int cur = cspace[m+1] - cspace[start];
                if (cur > room) {
                    r = m - 1;
                } else {
                    l = m;
                }
            }
            double val = cvalue[l+1] - cvalue[start] + score;
            room -= (cspace[l+1] - cspace[start]);
            if (l < n - 1) {
                auto [v, w, ind] = temp[l+1];
                int space = max(0LL, room);
                val += (v * space * 1.0) / w;
                room = 0;
            }
            assert(room == 0 || l == n - 1);
            assert(val >= 0);
            return val;
        };

        vi ans(n), cur(n);
        int iters = 0;
        ll best = mn;
        int opt = 1;
        function<ll(int, ll, int)> dfs;
        dfs = [&](int child, ll score, int room) -> ll {
            iters++;
            auto [v, w, ind] = temp[child];
            if (room < 0) return -1LL;
            if (child >= n) {
                best = max(best, score);
                return score;
            }
            if (iters > max_iters) {
                opt = 0;
                return -1LL;
            }

            int used_v = -1, unused_v = -1;
            double used_est = estimate(score + v, room - w, child + 1);
            double unused_est = estimate(score, room, child + 1);
            ll used_s = 0, unused_s = 0;
            if (used[child]) {
                if (used_est > best) {
                    used_s = dfs(child + 1, score + v, room - w);
                }
                if (unused_est > best) {
                    unused_s = dfs(child + 1, score, room);
                }
            } else {
                if (unused_est > best) {
                    unused_s = dfs(child + 1, score, room);
                }
                if (used_est > best) {
                    used_s = dfs(child + 1, score + v, room - w);
                }
            }
            int val = max(used_s, unused_s);
            if (val == best) {
                ans[ind] = used_s > unused_s;
            }
            return val;
        };

        dfs(0, 0, k);
        cout<<best<<" "<<opt<<endl;
        for (int i = 0; i < n; i++) {
            cout<<ans[i]<<" ";
        }
    }
};

int main() {
    Solver s = Solver(1e8);
    s.solve();
}