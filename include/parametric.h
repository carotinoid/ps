#ifndef PARAMETRIC_H
#define PARAMETRIC_H

#include <vector>
#include <functional>

template<typename T>
T parametric_search(long long low, long long high, const std::function<bool(T)> condition) {
    // low: inclusive, high: exclusive
    while (low + 1 < high) {
        long long mid = (low + high) / 2;
        if (condition(mid)) low = mid;
        else high = mid;
    }
    return low;
}

#endif // PARAMETRIC_H