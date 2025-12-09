#ifndef POLY_H
#define POLY_H

#include <vector>
#include <cmath>
#include <cassert>
#include <iostream>
#include <complex>
#include <algorithm>

#include "include/mint.h"

constexpr long long constexpr_pow(long long base, long long exp, long long mod) {
    long long res = 1;
    base %= mod;
    while (exp > 0) {
        if (exp % 2 == 1) res = (__int128)res * base % mod;
        base = (__int128)base * base % mod;
        exp /= 2;
    }
    return res;
}

constexpr int find_primitive_root(long long mod) {
    // NOT IMPLEMENTED
    return -1;
}

constexpr int get_primitive_root(long long mod) {
    if (mod == 998244353) return 3;
    else find_primitive_root(mod);
}

template <typename T>
struct poly_traits;

template <long long M>
struct poly_traits< mint<M> > {
    static constexpr long long mod = M;
    static constexpr int w = get_primitive_root(M);
};

template <typename T> // forward declaration
class poly;

using cpx = complex<double>;
using polycpx = vector<cpx>;
const double PI = acos(-1);

void fft(polycpx &a, bool inv = false) {
    // NOT IMPLEMENTED
}

template <long long mod, int w>
void ntt(poly<mint<mod>> &a, bool inv = false) {
    int n = a.size(), j = 0;
    assert((n & -n) == n); // n should be power of 2
    for(int i = 1; i < n; i++) {
        int bit = (n >> 1);
        while(j >= bit) {
            j -= bit;
            bit >>= 1;
        }
        j += bit;
        if(i < j) {
            swap(a[i], a[j]);
        }
    }

    vector<mint<mod>> roots(n/2);
    mint<mod> t = ((mint<mod>)(w)).pow((mod - 1) / n);
    if(inv) t = t.pow(mod - 2);
    for(int i = 0; i < n/2; i++) {
        if(i == 0) roots[i] = 1;
        else roots[i] = roots[i-1]*t;
    }
    for(int i = 2; i <= n; i <<= 1) {
        int step = n/i;
        for(int j = 0; j < n; j+=i) 
            for(int k = 0; k < i/2; k++) {
                mint<mod> u = a[j + k];
                mint<mod> v = a[j + k + i/2] * roots[step * k];
                a[j + k] = u + v;
                a[j + k + i/2] = u - v;
            }}
    mint<mod> inv_n = mint<mod>(n).inv();
    if(inv) for(int i = 0; i < n; i++) a[i] *= inv_n;
}

template <long long mod, int w>
poly<mint<mod>> conv(const poly<mint<mod>> &A, const poly<mint<mod>> &B) {
    if(A.size() == 0 || B.size() == 0) {
        return poly<mint<mod>>();
    }
    poly<mint<mod>> nA = A;
    poly<mint<mod>> nB = B;
    int conv_deg = A.deg() + B.deg();
    int m = 1;
    while(m <= conv_deg) m <<= 1;

    nA.resize(m); 
    nB.resize(m);
    ntt<mod,w>(nA); 
    ntt<mod,w>(nB);
    for(int i = 0; i < m; i++) nA[i] *= nB[i];
    ntt<mod,w>(nA, true);
    nA.resize(conv_deg + 1);

    return nA;
}



template <typename T>
class poly {
private:
    std::vector<T> V;
    static constexpr long long mod = poly_traits<T>::mod;
    static constexpr int w = poly_traits<T>::w;
    
public:
    poly() {}
    poly(T a) { V.push_back(a); }
    poly(const vector<T>& a): V(a) { truncate();}
    poly(initializer_list<T> list) : V(list) { truncate(); }
    explicit poly(int size) : V(size) {}
    poly(std::vector<T>&& v) : V(std::move(v)) { truncate(); }
    
    bool empty() const { return V.empty(); }
    void clear() { V.clear(); }
    void push_back(const T& a) { V.push_back(a); }
    poly& resize(int n) { V.resize(n, T(0)); return *this; }
    int size() const { return V.size(); }
    int deg() const { return size() - 1; }
    poly& truncate() {
        while (!V.empty() && V.back() == T(0)) V.pop_back();
        return *this;
    }
    auto begin() { return V.begin(); }
    auto end() { return V.end(); }
    auto begin() const { return V.begin(); }
    auto end() const { return V.end(); }
    
    T& operator[](int k) {
        if(k >= size()) V.resize(k + 1, T(0));
        return V[k];
    }

    const T& operator[](int k) const {
        if (k < 0 || k >= size()) return T(0);
        return V[k];
    }
    
    static poly singleton(int k) {
        poly<T> res;
        res.resize(k + 1);
        res[k] = 1;
        return res;
    }
    
    poly operator+() const { return *this; }
    poly operator-() const {
        poly<T> res = *this;
        for(auto &x : res.V) x = -x;
        return res;
    }
    poly& negate() {
        for(auto &x : V) x = -x;
        return *this;
    }
    poly& operator+=(const T& rhs) {
        if(empty()) V.resize(1, T(0));
        else V[0] += rhs;
        return truncate();
    }
    poly& operator-=(const T& rhs) {
        if(empty()) V.resize(1, T(0));
        else V[0] -= rhs;
        return truncate();
    }
    poly& operator*=(const T& rhs) {
        for(auto &x : V) x *= rhs;
        return *this;
    }
    poly& operator/=(const T& rhs) { return *this *= rhs.inv();}    
    
    poly& operator+=(const poly<T>& rhs) {
        if(size() < rhs.size()) V.resize(rhs.size(), T(0));
        for(int i = 0; i < rhs.size(); i++) V[i] += rhs[i];
        return truncate();
    }
    poly& operator-=(const poly<T>& rhs) {
        if(size() < rhs.size()) V.resize(rhs.size(), T(0));
        for(int i = 0; i < rhs.size(); i++) V[i] -= rhs[i];
        return truncate();
    }
    poly& operator*=(const poly<T>& rhs) { return *this = conv<mod, w>(*this, rhs); }
    poly& operator/=(const poly<T>& rhs) {
        poly<T> rhs_inv = rhs.inv(size());
        return *this *= rhs_inv;
    }

    friend poly operator+(poly lhs, const poly& rhs) { return lhs += rhs; }
    friend poly operator-(poly lhs, const poly& rhs) { return lhs -= rhs; }
    friend poly operator+(poly lhs, const T& rhs) { return lhs += rhs; }
    friend poly operator-(poly lhs, const T& rhs) { return lhs -= rhs; }
    friend poly operator*(poly lhs, const poly& rhs) { return lhs *= rhs; }
    friend poly operator*(poly lhs, const T& rhs) { return lhs *= rhs; }
    friend poly operator*(const T& lhs, poly rhs) { return rhs *= lhs; }
    friend poly operator/(poly lhs, const T& rhs) { return lhs /= rhs; }

    poly& operator<<=(int k) {
        assert(k >= 0);
        V.resize(size() + k);
        for (int i = size() - 1 - k; i >= 0; i--) V[i + k] = V[i];
        for (int i = 0; i < k; i++) V[i] = T(0);
        return *this;
    }
    
    poly& operator>>=(int k) {
        assert(k >= 0);
        if (size() <= k) { V.clear(); return *this; }
        for (int i = 0; i < size() - k; i++) V[i] = V[i + k];
        V.resize(size() - k);
        return *this;
    }
    friend poly operator<<(poly a, int k) { return a <<= k; }
    friend poly operator>>(poly a, int k) { return a >>= k; }

    poly& operator%=(int sz) { V.resize(sz); return *this; }
    friend poly operator%(poly a, int sz) { return a %= sz; }
    

    poly reverse() const {
        poly<T> res = *this;
        std::reverse(res.V.begin(), res.V.end());
        return res;
    }

    poly derivate() const {
        if (size() <= 1) return poly();
        std::vector<T> res(size() - 1);
        for (int i = 1; i < size(); ++i) res[i - 1] = V[i] * T(i);
        return poly(std::move(res));
    }
    poly integrate() const {
        if (empty()) return poly();
        std::vector<T> res(size() + 1);
        res[0] = T(0);
        for (int i = 0; i < size(); ++i) res[i + 1] = V[i] / T(i + 1);
        return poly(std::move(res));
    }
    poly inv(int t) const {
        assert(V[0] != T(0));
        poly<T> f = *this, g = poly<T>(1 / V[0]), h;
        f %= t;
        // g = g * (- (g * (f % i) % i) + T(2)) % i;
        for(int i = 2; i <= 2 * t; i <<= 1) {
            h = (f % i); h *= g; h %= i; h.negate();
            h[0] += T(2); h *= g; h %= i; g = h;
        }
        g %= t;
        return g;
    }
    poly log(int t) const {
        poly<T> f = *this;
        poly<T> g, h;
        // (f.derivate() * f.inv(t)).integrate() % t;
        g = f.derivate(); h = f.inv(t); h *= g;
        h %= t; h = h.integrate(); h %= t;
        return h;
    }

    poly exp(int t) {
        assert(V[0] == T(0));
        poly<T> g = singleton(0);
        poly<T> f = *this;
        f %= t;
        poly<T> h;
        for(int i = 2; i <= 2 * t; i <<= 1) {
            // g = g * ((f % i) + T(1) - (g.log(i))) % i;
            h = f; h %= i; h[0] += T(1); h -= g.log(i); 
            h *= g; h %= i; g = h;
        }
        g %= t;
        return g;
    }

    poly pow(long long k, int t) {
        if(k == 0) { return poly<T>{T(1)}.resize(t); }
        int idx = 0;
        while(idx < size()) 
            if(V[idx] != T(0)) break;
            else idx++;   
        if(idx == size() || (idx > 0 && k >= t) || (idx > 0 && idx * k >= t))
            return poly<T>().resize(t); 
        T c = V[idx];
        T c_inv = c.inv();
        poly<T> f = *this;
        poly<T> g = f >> idx;
        for(auto &x : g.V) x *= c_inv;
        poly<T> ln_g = g.log(t - idx * k);
        ln_g *= T(k);
        poly<T> res = ln_g.exp(t - idx * k);
        T ck = c.pow(k);
        for(auto &x : res.V) x *= ck;
        poly<T> final_res = res << (idx * k);
        final_res.resize(t);
        return final_res;
    }

    pair<poly,poly> divide_and_remainder(poly g) const {
        assert(!g.empty());
        poly f = *this;
        if(f.deg() < g.deg()) return {poly<T>(), f}; 
        poly q = f.reverse();
        q /= g.reverse();
        int t = f.deg() - g.deg() + 1;
        q %= (t);
        q = q.reverse();
        poly r = f - q * g;
        return {q.truncate(), r.truncate()};
    }

    poly remainder(const poly& g) const {
        assert(!g.empty());
        poly f = *this;
        if(f.deg() < g.deg()) return f; 

        if(g.deg() <= 32 || f.deg() - g.deg() <= 32) {
            poly r = f;
            int fd = f.deg(), gd = g.deg();
            T g_inv = T(1) / g[gd];
            
            for(int i = fd; i >= gd; i--) {
                T coef = r[i] * g_inv;
                for(int j = 0; j <= gd; j++) {
                    r[i - gd + j] -= coef * g[j];
                }
            }
            r.V.resize(gd);
            return r.truncate();
        }

        return f.divide_and_remainder(g).second;
    }

    poly taylor_shift(T c) const {
        int sz = size();
        T fac = T(1);
        poly A = *this;
        poly C = poly().resize(sz);
        for(int i = 0; i < sz; i++) {
            A[i] *= fac;
            C[i] = c.pow(i) / fac;
            fac *= T(i + 1);
        }
        poly B = C * A.reverse();
        B %= sz;
        B = B.reverse();
        fac = T(1);
        poly res = poly().resize(sz);
        for(int i = 0; i < sz; i++) {
            res[i] = B[i] / fac;
            fac *= T(i + 1);
        }
        return res;
    }

    vector<T> multipoint_evaluation(const vector<T>& points) const {
        int n = (int)points.size();
        if (n == 0) return {};
        if (empty()) return vector<T>(n, T(0));

        if (n <= 64) {
            vector<T> results(n);
            for (int i = 0; i < n; i++) {
                T x = points[i];
                T val = T(0);
                T power = T(1);
                for (int j = 0; j < size(); j++) {
                    val += V[j] * power;
                    power *= x;
                }
                results[i] = val;
            }
            return results;
        }

        vector<poly> tree(4 * n);

        auto build = [&](auto&& self, int node, int l, int r) -> void {
            if (l == r) { tree[node] = poly({-points[l], T(1)}); return; }
            int mid = (l + r) / 2;
            self(self, node * 2, l, mid);
            self(self, node * 2 + 1, mid + 1, r);
            
            tree[node] = tree[node * 2] * tree[node * 2 + 1];
        };
        build(build, 1, 0, n - 1);
        vector<T> results(n);        
        poly start_poly = this->remainder(tree[1]); 

        auto solve = [&](auto&& self, int node, int l, int r, const poly& f) -> void {
            if (f.empty()) { for (int i = l; i <= r; i++) results[i] = T(0); return; }
            if (l == r) { results[l] = f[0]; return; }
            int mid = (l + r) / 2;
            self(self, node * 2, l, mid, f.remainder(tree[node * 2]));
            self(self, node * 2 + 1, mid + 1, r, f.remainder(tree[node * 2 + 1]));
        };
        solve(solve, 1, 0, n - 1, start_poly);
        return results;
    }

    poly lagrange_interpolation(); // NOT IMPLEMENTED

    friend ostream& operator<<(ostream& os, const poly<T>& p) {
        for(int i = 0; i < p.size(); i++) 
            os << (p.V[i]) << (i != p.size() - 1 ? " " : "");
        return os; 
    }
};

template <typename T> poly<T> derivate(poly<T>& p) { return p.derivate(); }
template <typename T> poly<T> integrate(poly<T>& p) { return p.integrate(); }
template <typename T> poly<T> inv(poly<T>& p, int t) { return p.inv(p.size(), t); }
template <typename T> poly<T> log(poly<T>& p, int t) { return p.log(t); }
template <typename T> poly<T> exp(poly<T>& p, int t) { return p.exp(t); }
template <typename T> poly<T> pow(poly<T>& p, long long k, int t) { return p.pow(k, t); }
template <typename T> pair<poly<T>, poly<T>> divide_and_remainder(const poly<T>& f, const poly<T>& g) { return f.divide_and_remainder(g); }
template <typename T> poly<T> remainder(const poly<T>& f, const poly<T>& g) { return f.remainder(g); }
template <typename T> poly<T> taylor_shift(const poly<T>& f, T c) { return f.taylor_shift(c); }
template <typename T> vector<T> multipoint_evaluation(const poly<T>& f, const vector<T>& points) { return f.multipoint_evaluation(points); }


#endif // POLY_H
