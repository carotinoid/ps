#include <bits/stdc++.h>
#define forr(i, n) for(ll i=1;i<=(n);i++)
#define fors(i, s, e) for(ll i=(s);i<=(e);i++)
#define fore(i, e, s) for(ll i=(e);i>=(s);i--)
#define getint(a) ll a; cin>>a;
#define getints(a, b) ll a,b; cin>>a>>b;
#define getll(a) ll a; cin>>a;
#define getlls(a, b) ll a,b; cin>>a>>b;
#define getstr(s) string s; cin >> s;
#define all(v) v.begin(), v.end()
#define endl '\n' 
#define fi first
#define se second
#define pb push_back
#define set_decimal(n) cout<<fixed;cout.precision(n);
#define len(v) ll((v).size())
#define _ <<" "<<
#define next next_
using namespace std;
using ll = long long; using ld = long double;
using point = pair<ld,ld>;
// const long long mod = 1'000'000'007;
const long long mod = 998'244'353;
void setup();
void solve();
mt19937_64 rng(chrono::steady_clock::now().time_since_epoch().count());

/* Features */
#ifdef DEBUGTOOLS
#include "include/debugtools.h" // should be excluded
#endif
// #include "include/pbds.h"
// #include "include/fastio.h"
#include "include/mint.h"
#include "include/polynomial.h"
// #include "include/fps.h"

/* Configuration finalization */
#ifndef FASTIO_H
#define fastio \
cin.tie(0) -> sync_with_stdio(0); setup();
#else
#define fastio 42
#endif
#ifndef DEBUGTOOLS_H
void DEBUG() {}
template <typename T,typename... Args>
void DEBUG(T first, Args... args) {DEBUG(args...);} 
enum class COLOR: int {RED=0,GREEN,ORANGE,BLUE,PURPLE,CYAN,YELLOW,COUNT};
#endif

/* Write code here*/

using mint998244353 = mint<998'244'353>;

void solve()
{
    poly<mint998244353> A;
    getint(n);
    forr(i, n) {getint(t); A.pb(t);}
    cout << A.log() << endl;
}

void setup() {}
int main()
{
    fastio;
    // getint(n); forr(i, n)
    solve();
}
