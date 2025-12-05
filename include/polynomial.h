#ifndef POLY_H
#define POLY_H

#include <vector>
#include <cmath>
#include <cassert>
#include <iostream>
#include <complex>

using namespace std;

template <typename T>
class poly {
private:
    vector<T> V;
    poly& remove_leading_zeros() {
        while (!V.empty() && V.back() == T(0))
            V.pop_back();
        return *this;
    }
    poly& truncate(int n) {
        if (n < size()) {
            V.resize(n);
        }
        return *this;
    }

public:
    poly() {}
    poly(T a) { V.push_back(a); }
    poly(const vector<T>& a): V(a) {}

    void push_back(const T& a) { V.push_back(a); }
    void resize(int n) { V.resize(n); }
    int size() const { return V.size(); }
    int deg() const { return size() - 1; }
    T& operator[](int k) {
        assert(0 <= k);
        if(k >= size()) V.resize(k + 1);
        return V[k];
    }
    poly operator+() const { return *this; }
    poly operator-() const {
        poly<T> res = *this;
        for(int i = 0; i < res.size(); i++) res.V[i] = -res.V[i];
        return res;
    }

    poly& operator+=(const poly<T>& a) {
        for(int i = 0; i < a.size(); i++) (*this)[i] += a.V[i];
        this->remove_leading_zeros();
        return *this;
    }
    poly& operator-=(const poly<T>& a) {
        for(int i = 0; i < a.size(); i++) (*this)[i] -= a.V[i];
        this->remove_leading_zeros();
        return *this;
    }
    poly& operator*=(const poly<T>& a) {
        *this = conv(*this, a);
        return *this;
    }
    poly& operator/=(const poly<T>& a) {
        *this = conv(*this, a.inv());
        return *this;
    }

    friend poly<T> operator+(const poly<T>& a, const poly<T>& b) {
        poly<T> res = a;
        return res += b;
    }
    friend poly<T> operator-(const poly<T>& a, const poly<T>& b) {
        poly<T> res = a;
        return res -= b;
    }

    friend poly<T> operator*(const poly<T>& a, const poly<T>& b) {
        poly<T> res = a;
        return res *= b;
    }

    friend poly<T> operator/(const poly<T>& a, const poly<T>& b) {
        // NOT IMPLEMENTED
        return poly<T>();
    }

    poly inv() const {
        assert(V[0] != 0);
        poly<T> cpy = *this;
        poly<T> f;
        f.push_back(1 / V[0]);
        int n = size();
        for(int i = 1; i < n; i <<= 1) {
            poly<T> a = *this;
            a.resize(2 * i);
            poly<T> g = conv(a, f);
            g.resize(2 * i);
            g = -g;
            g[0] += (T)2;
            f = conv(f, g);
            f.resize(2 * i);
        }
        return f.truncate(n);
    }

    poly derivate() {
        poly<T> res;
        int n = size();
        if(n == 0) return res;
        res.resize(n - 1);
        for(int i = 1; i < n; i++) {
            res[i - 1] = V[i] * T(i);
        }
        return res;
    }
    poly integrate() {
        poly<T> res;
        int n = size();
        res.resize(n + 1);
        res[0] = T(0);
        for(int i = 0; i < n; i++) {
            res[i + 1] = V[i] / T(i + 1);
        }
        return res;
    }
    poly log() {
        poly<T> f_prime = this->derivate();
        poly<T> f = *this;
        poly<T> g = f_prime / f;
        return g.integrate();
    }
    poly exp(); // NOT IMPLEMENTED

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


using cpx = complex<double>;
using polycpx = vector<cpx>;
const double PI = acos(-1);

void fft(polycpx &a, bool inv = false) {
    // NOT IMPLEMENTED
}

template <long long mod>
void ntt_nonrecursive(poly<mint<mod>> &a, int w, bool inv = false) {
    // from GMS
    int n = a.size(), j = 0;
    assert((n & -n) == n); // n is power of 2
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

    poly<mint<mod>> roots;
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

template <long long mod>
void ntt(poly<mint<mod>> &a, int w, bool inv = false) {
    ntt_nonrecursive<mod>(a, w, inv);
    // ntt_recursive(a, inv); // NOT IMPLEMENTED
}


poly<mint<mod>> conv(const poly<mint<mod>> &A, const poly<mint<mod>> &B) {
    poly<mint<mod>> nA = A;
    poly<mint<mod>> nB = B;
    constexpr int w = 3;
    int conv_deg = A.deg() + B.deg();
    int conv_size = conv_deg + 1;
    int m = 1;
    while(m < conv_size) m <<= 1;

    nA.resize(m); nB.resize(m);
    ntt(nA, w); ntt(nB, w);
    for(int i = 0; i < m; i++) nA[i] *= nB[i];
    ntt(nA, w, true);
    nA.resize(conv_size);

    return nA;
}


#endif // POLY_H
