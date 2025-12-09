#ifndef MINT_H
#define MINT_H // modular integer

#include <iostream>
#include <vector>
#include <cassert>
#include <algorithm>

template <long long mod>
class mint {
private:
    long long _v;

public:
    constexpr mint() noexcept : _v(0) {}
    constexpr mint(int a) noexcept : _v(a % mod) {if (_v < 0) _v += mod;}
    constexpr mint(long long a) noexcept : _v(a % mod) {if (_v < 0) _v += mod;}

    constexpr long long  val() const noexcept { return _v; }
    constexpr mint pow(long long k) const noexcept {
        assert(0 <= k);
        mint x = *this, r = 1;
        while (k) {
            if (k & 1) r *= x;
            x *= x;
            k >>= 1;
        }
        return r;
    }
    constexpr mint inv() const noexcept {
        long long a = _v, b = mod;
        long long u = 1, v = 0;
        while (b) {
            long long t = a / b;
            a -= t * b; std::swap(a, b);
            u -= t * v; std::swap(u, v);
        }
        assert(a == 1);     
        return mint(u);
    }
    constexpr mint operator+() const noexcept { return *this; }
    constexpr mint operator-() const noexcept { return mint() - *this; }
    constexpr mint& operator++() noexcept {
        _v ++;
        if (_v == mod) _v = 0;
        return *this;
    }
    constexpr mint& operator--() noexcept {
        _v --;
        if (_v == -1) _v = mod - 1;
        return *this;
    }
    constexpr mint operator++(int) noexcept {
        mint temp = *this;
        ++*this;
        return temp;
    }
    constexpr mint operator--(int) noexcept {
        mint temp = *this;
        --*this;
        return temp;
    }
    constexpr mint& operator+=(const mint& a) noexcept {
        _v += a._v;
        if (_v >= mod) _v -= mod;
        return *this;
    }
    constexpr mint& operator-=(const mint& a) noexcept {
        _v -= a._v;
        if (_v < 0) _v += mod;
        return *this;
    }
    constexpr mint& operator*=(const mint& a) noexcept {
        _v = _v * a._v % mod;
        return *this;
    }
    constexpr mint& operator/=(const mint& a) noexcept {
        return *this *= a.inv();
    }

    friend mint operator+(const mint& lhs, const mint& rhs) {
        return mint(lhs) += rhs;
    }
    friend mint operator-(const mint& lhs, const mint& rhs) {
        return mint(lhs) -= rhs;
    }
    friend mint operator*(const mint& lhs, const mint& rhs) {
        return mint(lhs) *= rhs;
    }
    friend mint operator/(const mint& lhs, const mint& rhs) {
        return mint(lhs) /= rhs;
    }
    friend bool operator==(const mint& lhs, const mint& rhs) {
        return lhs._v == rhs._v;
    }
    friend bool operator!=(const mint& lhs, const mint& rhs) {
        return lhs._v != rhs._v;
    }
    friend istream& operator>>(istream& is, mint& a) {
        long long num;
        is >> num;
        a._v = num % mod;
        return is;
    }
    friend ostream& operator<<(ostream& os, const mint& a) {
        return os << a.val();
    }

};

template <long long mod>
mint<mod> pow(const mint<mod>& a, long long k) { return a.pow(k); }
template <long long mod>
mint<mod> pow(const mint<mod>& a, int k) { return a.pow(k); }

#endif // MINT_H
