#include <random>
#include <chrono>
#define fastio \
cin.tie(0) -> sync_with_stdio(0); setup();
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
using ll = long long; using ld = long double;
using point = std::pair<ld,ld>;
std::mt19937_64 rng(std::chrono::steady_clock::now().time_since_epoch().count());