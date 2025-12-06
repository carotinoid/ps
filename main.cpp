#include <bits/stdc++.h>
#include "include/base.h"
#include "include/debugtools.h"
#include "include/mint.h"
#include "include/polynomial.h"
// #include "include/fastio.h"
// #include "include/pbds.h"

using namespace std;
using ll = long long;

constexpr long long mod = 998'244'353;

using mint998244353 = mint<998'244'353>;

void solve()
{
    poly<mint998244353> A;
    getint(n); n++;
    forr(i, n) {getint(t); A.pb(t);}
    cout << (A + mint998244353(1)).log(n) << endl;
    cout << (A.exp(n) - mint998244353(1)).resize(n) << endl;
}

int main()
{
    fastio;
    // getint(n); forr(i, n)
    solve();
}
