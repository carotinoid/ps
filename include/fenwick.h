#ifndef FENWICK_H
#define FENWICK_H

#include <vector>
#include <iostream>

template <typename T>
class Fenwick {
private:
    std::vector<T> arr;
    std::vector<T> tree;
    inline unsigned size() const { return arr.size(); }
    void _update(int idx, T val) {
        arr[idx] += val;
        while(idx < size()) {
            tree[idx] += val;
            idx = idx + (idx & -idx);
        }
    }
    void _set(int idx, T val) {
        T cur = arr[idx];
        T delta = val - cur;
        _update(idx, delta);
    }
    T _get_prefix(int idx) {
        T sum = 0;
        while(idx > 0) {
            sum += tree[idx];
            idx = idx - (idx & -idx);
        }
        return sum;
    }
    T _get_range(int l, int r) { return _get_prefix(r) - _get_prefix(l - 1); }

public:
    /**
     * @brief Constructor for Fenwick Tree
     * @param input Input array (1-indexed)
     */
    Fenwick(std::vector<T> &input) {
        arr.resize(input.size() + 1, 0);
        tree.resize(input.size() + 1, 0);
        for(int i = 1; i < input.size(); i++) {
            _update(i, input[i]);
        }
    }
    /**
     * @brief Function to update the Fenwick Tree
     * @param idx Index to update (1-indexed)
     * @param val Value to add
     * @return void
     */
    void update(int idx, T val) { _update(idx, val); }
    /**
     * @brief Function to set value at idx
     * @param idx Index to set (1-indexed)
     * @param val Value to set
     * @return void
     */
    void set(int idx, T val) { _set(idx, val); }
    /**
     * @brief Function to get prefix sum up to idx
     * @param idx Index to get sum up to (1-indexed)
     * @return int Prefix sum
     */
    T get_prefix(int idx) { return _get_prefix(idx); }
    /**
     * @brief Function to get range sum from l to r
     * @param l Left index (1-indexed)
     * @param r Right index (1-indexed)
     * @return int Range sum
     */
    T get_range(int l, int r) { return _get_range(l, r); }
    /**
     * @brief Function to get value at idx
     * @param idx Index to get value at (1-indexed)
     * @return int Value at idx
     */
    T get_value(int idx) { return arr[idx]; }
    /**
     * @brief Function to print the Fenwick Tree (for debugging, dummy is truncated)
     */
    void print_tree() {
        using namespace std;
        bool f = true;
        for(auto &x : tree) {
            if(f) {f = false; continue;}
            cout << x << " ";
        }
        cout << endl;
    }
    /**
     * @brief Function to print the original array (for debugging, dummy is truncated)
     */
    void print_array() {
        using namespace std;
        bool f = true;
        for(auto &x : arr) {
            if(f) {f = false; continue;}
            cout << x << " ";
        }
        cout << endl;
    }
};

#endif // FENWICK_H
