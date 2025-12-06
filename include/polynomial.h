#ifndef POLY_H
#define POLY_H

#include <vector>
#include <cmath>
#include <cassert>
#include <iostream>
#include <complex>
#include <algorithm>

#include "include/mint.h"

using namespace std;

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
    vector<T> V;
    static constexpr long long mod = poly_traits<T>::mod;
    static constexpr int w = poly_traits<T>::w;
    
public:
    poly() {}
    poly(T a) { V.push_back(a); }
    poly(const vector<T>& a): V(a) { truncate();}
    
    poly& truncate() {
        while (!V.empty() && V.back() == T(0)) V.pop_back();
        return *this;
    }
    poly truncate(int n) {
        poly<T> res(n);
        for(int i = 0; i < n; i++) res[i] = (*this)[i];
        return res;
    }

    void push_back(const T& a) { V.push_back(a); }
    poly& resize(int n) { V.resize(n); return *this; }
    int size() const { return V.size(); }
    int deg() const { return size() - 1; }

    T& operator[](int k) {
        if(k >= size()) V.resize(k + 1);
        return V[k];
    }

    const T& operator[](int k) const {
        if (k >= size()) { static T zero = T(0); return zero; }
        return V[k];
    }

    poly operator+() const { return *this; }
    poly operator-() const {
        poly<T> res = *this;
        for(auto &x : res.V) x = -x;
        return res;
    }

    poly& operator+=(const poly<T>& a) {
        if(size() < a.size()) V.resize(a.size());
        for(int i = 0; i < a.size(); i++) V[i] += a[i];
        return truncate();
    }
    poly& operator-=(const poly<T>& a) {
        if(size() < a.size()) V.resize(a.size());
        for(int i = 0; i < a.size(); i++) V[i] -= a[i];
        return truncate();
    }
    poly operator%=(const int i) {
        return V.truncate(i);
    }

    poly& operator*=(const poly<T>& a) {
        *this = conv<mod, w>(*this, a);
        return *this;
    }

    friend poly<T> operator+(const poly<T>& a, const poly<T>& b) { poly<T> res = a; return res += b; }
    friend poly<T> operator-(const poly<T>& a, const poly<T>& b) { poly<T> res = a; return res -= b; }
    friend poly<T> operator*(const poly<T>& a, const poly<T>& b) { poly<T> res = a; return res *= b; }
    friend poly<T> operator%(const poly<T>& a, const poly<T>& b) { poly<T> res = a; return res %= b; }

    poly inv(int t) const {
        assert(V[0] != 0);
        poly<T> cpy = *this;
        poly<T> f;
        f.push_back(1 / V[0]);
        int n = size();
        for(int i = 1; i < t; i <<= 1) {
            poly<T> a = *this;
            a.resize(2 * i);
            poly<T> g = conv<mod, w>(a, f);
            g.resize(2 * i);
            g = -g;
            g[0] += (T)2;
            f = conv<mod, w>(f, g);
            f.resize(2 * i);
        }
        return f.truncate(t);
    }
    poly derivate() const {
        poly<T> res;
        int n = size();
        if(n == 0) return res;
        res.resize(n - 1);
        for(int i = 1; i < n; i++) {
            res[i - 1] = V[i] * T(i);
        }
        return res;
    }
    poly integrate() const {
        poly<T> res;
        int n = size();
        res.resize(n + 1);
        res[0] = T(0);
        for(int i = 0; i < n; i++) {
            res[i + 1] = V[i] / T(i + 1);
        }
        return res;
    }
    poly log(int t) {
        poly<T> f_prime = this->derivate();
        poly<T> f = *this;
        poly<T> g = f_prime * f.inv(t);
        return g.integrate().truncate(t);
    }
    poly exp(int t) {
        assert(V[0] == 0);
        poly<T> g0 = poly<T>(1);
        for(int i = 1; i < t;) {
            i <<= 1;
            poly<T> f1 = this->truncate(i);
            poly<T> f2 = T(1);
            poly<T> f3 = g0.log(i);
            poly<T> f = f1 + f2 - f3;
            g0 = (g0 * f).truncate(i);
        }
        return g0.truncate(t);
    }
    void print() {
        for (int i = 0; i < size(); i++) {
            if(i == 0) cout << V[i] << " ";
            else if(i == 1) cout << "+ " << V[i] << "x ";
            else cout << "+ " << V[i] << "x^" << i << " ";
        }
        cout << endl;
    }
    friend ostream& operator<<(ostream& os, const poly<T>& p) {
        for(int i = 0; i < p.size(); i++) {
            os << (p.V[i]) << (i != p.size() - 1 ? " " : "");
        }
        return os; 
    }
};

template <typename T>
poly<T> log(poly<T>& p, int t) {
    return p.log(t);
}

#endif // POLY_H
