#ifndef MINT_H
#define MINT_H // modular integer

template <long long mod>
class mint {
private:
    unsigned int _v;

public:
    mint(): _v(0) {}
    mint(int a): _v(a % mod) {}
    mint(long long a): _v (a % mod) {}
    
    int modulus() const { return mod; }
    int val() const { return _v; }
    mint pow(long long k) {
        assert(0 <= k);
        mint x = *this, r = 1;
        while (k) {
            if (k & 1) r *= x;
            x *= x;
            k >>= 1;
        }
        return r;
    }
    mint inv() const {
        long long a = _v, b = mod;
        long long u = 1, v = 0;
        while (b) {
            long long t = a / b;
            a -= t * b; std::swap(a, b);
            u -= t * v; std::swap(u, v);
        }
        assert(a == 1);     
        if (u < 0) u += mod;
        return mint(u);
    }
    mint operator+() const { return *this; }
    mint operator-() const { return mint() - *this; }
    mint& operator++() {
        _v = (_v + 1) % mod;
        return *this;
    }
    mint& operator--() {
        _v = (_v + mod - 1) % mod;
        return *this;
    }
    mint& operator+=(const mint& a) {
        _v = (_v + a._v) % mod;
        return *this;
    }
    mint& operator-=(const mint& a) {
        _v = (_v + mod - a._v) % mod;
        return *this;
    }
    mint& operator*=(const mint& a) {
        _v = (unsigned long long)(_v) * a._v % mod;
        return *this;
    }
    mint& operator/=(const mint& a) {
        return *this = *this * a.inv();
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
        int num;
        is >> num;
        a._v = num;
        return is;
    }
    friend ostream& operator<<(ostream& os, const mint& a) {
        return os << a.val();
    }

};

#endif // MINT_H