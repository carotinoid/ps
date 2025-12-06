#ifndef FASTIO_H
#define FASTIO_H

#include <sys/mman.h>   
#include <sys/stat.h>
#include <unistd.h>
#include <string>
#include <iostream>

class FastIO {
    char* p;
    char* end;
public:
    FastIO() {
        struct stat st;
        fstat(0, &st);
        if (st.st_size == 0) {
            p = end = nullptr;
        } else {
            p = (char*)mmap(0, st.st_size, PROT_READ, MAP_SHARED, 0, 0);
            if (p == MAP_FAILED)
                p = end = nullptr;
            else 
                end = p + st.st_size;
        }
    }

    ~FastIO() {
        if (p && p != MAP_FAILED) {
            munmap(p, end - p);
        }
    }
    
    template <typename T>
    inline T readInt() {
        T ret = 0;
        bool neg = false;
        while (p < end && *p <= ' ') p++;
        if (p >= end) return 0;
        if (*p == '-') {
            neg = true;
            p++;
        }
        while (p < end && *p >= '0' && *p <= '9') 
            ret = ret * 10 + (*p++ - '0');
        return neg ? -ret : ret;
    }

    template <typename T>
    inline T readReal() {
        T ret = 0.0;
        bool neg = false;
        while (p < end && *p <= ' ') p++;
        if (p >= end) return 0.0;
        if (*p == '-') {
            neg = true;
            p++;
        }
        while (p < end && *p >= '0' && *p <= '9') 
            ret = ret * 10 + (*p++ - '0');
        if (p < end && *p == '.') {
            p++;
            T multiplier = 0.1;
            while (p < end && *p >= '0' && *p <= '9') {
                ret += (*p++ - '0') * multiplier;
                multiplier *= 0.1;
            }
        }
        return neg ? -ret : ret;
    }

    template <typename T>
    inline T readByte() {
        while (p < end && *p <= ' ') p++;
        if (p >= end) return 0;
        return *p++;
    }

    inline std::string readString() {
        std::string ret;
        while (p < end && *p <= ' ') p++;
        if (p >= end) return ret;
        char* start = p;
        while (p < end && *p > ' ') p++;
        return std::string(start, p);
    }
    
    inline std::string readLine() {
        if (p >= end) return "";
        char* start = p;
        while (p < end && *p != '\n') p++;
        char* str_end = p;
        if (str_end > start && *(str_end - 1) == '\r')
            str_end--;
        std::string ret(start, str_end);
        if (p < end) p++;      
        return ret;
    }

    inline void ignore() {
        while (p < end && *p != '\n') p++;
        if (p < end) p++;
    }

    inline FastIO& operator>>(int& x) { x = readInt<int>(); return *this; }
    inline FastIO& operator>>(long long& x) { x = readInt<long long>(); return *this; }
    inline FastIO& operator>>(double& x) { x = readReal<double>(); return *this; }
    inline FastIO& operator>>(float& x) { x = readReal<float>(); return *this; }
    inline FastIO& operator>>(long double& x) { x = readReal<long double>(); return *this; }
    inline FastIO& operator>>(char& c) { c = readByte<char>(); return *this; }
    inline FastIO& operator>>(std::string& s) { s = readString(); return *this; }
    inline FastIO& getline(std::string& s) { s = readLine(); return *this; }
};

static FastIO _fast_io_instance;

#define cin _fast_io_instance
#define fastio 42 // Redefined existing macro

#endif