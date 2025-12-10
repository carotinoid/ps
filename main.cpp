#include <bits/stdc++.h>
#include "include/base.h"
#include "include/debugtools.h"
#include "include/mint.h"
#include "include/polynomial.h"
// #include "include/fastio.h"
// #include "include/pbds.h"

using namespace std;
constexpr long long mod = 998'244'353;
using mint998244353 = mint<998244353>;

void solve() 
{
    getints(n, pts);
    n++;
    poly<mint998244353> P;
    vector<mint998244353> vals;
    forr(i, n) {getint(t); P.pb(t);}
    forr(i, pts) {getint(t); vals.pb(t);}
    reverse(P.begin(), P.end());
    vector<mint998244353> res = P.multipoint_evaluation(vals);
    for (auto x : res) {
        cout << x << endl;
    }
}

int main()
{
    fastio;
    // getint(n); forr(i, n)
    solve();
}
