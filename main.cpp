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
    getints(n, m);
    n++;
    poly<mint998244353> C;
    vector<mint998244353> pts;
    forr(i, n) {
        getint(a);
        C.pb(a);
    }
    forr(i, m) {
        getint(x);
        pts.pb(x);
    }
    C = C.reverse();
    vector<mint998244353> res = multipoint_evaluation(C, pts);
    for(auto &x: res) {
        cout << x << " ";
    }
}

int main()
{
    fastio;
    // getint(n); forr(i, n)
    solve();
}
