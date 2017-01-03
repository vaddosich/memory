#ifndef BIT_MANIP
#define BIT_MANIP
#include <stddef.h>
//// bit ////
#ifdef _MSC_VER
#ifdef _WIN32
inline unsigned int __builtin_ctz(unsigned int x) { unsigned long r; _BitScanForward(&r, x); return r; }
inline unsigned int __builtin_clz(unsigned int x) { unsigned long r; _BitScanReverse(&r, x); return 31 - r; }
inline unsigned int __builtin_ffs(unsigned int x) { unsigned long r; return _BitScanForward(&r, x) ? r + 1 : 0; }
inline unsigned int __builtin_popcount(unsigned int x){ return __popcnt(x); }
#ifdef _WIN64
inline unsigned long long __builtin_ctzll(unsigned long long x) { unsigned long r; _BitScanForward64(&r, x); return r; }
inline unsigned long long __builtin_clzll(unsigned long long x) { unsigned long r; _BitScanReverse64(&r, x); return 63 - r; }
inline unsigned long long __builtin_ffsll(unsigned long long x) { unsigned long r; return _BitScanForward64(&r, x) ? r + 1 : 0; }
inline unsigned long long __builtin_popcountll(unsigned long long x) { return __popcnt64(x); }
#else
inline unsigned int hidword(unsigned long long x) { return static_cast<unsigned int>(x >> 32); }
inline unsigned int lodword(unsigned long long x) { return static_cast<unsigned int>(x & 0xFFFFFFFF); }
inline unsigned long long __builtin_ctzll(unsigned long long x) { return lodword(x) ? __builtin_ctz(lodword(x)) : __builtin_ctz(hidword(x)) + 32; }
inline unsigned long long __builtin_clzll(unsigned long long x) { return hidword(x) ? __builtin_clz(hidword(x)) : __builtin_clz(lodword(x)) + 32; }
inline unsigned long long __builtin_ffsll(unsigned long long x) { return lodword(x) ? __builtin_ffs(lodword(x)) : hidword(x) ? __builtin_ffs(hidword(x)) + 32 : 0; }
inline unsigned long long __builtin_popcountll(unsigned long long x) { return __builtin_popcount(lodword(x)) + __builtin_popcount(hidword(x)); }
#endif // _WIN64
#endif // _WIN32
#endif // _MSC_VER

inline unsigned int bsf(unsigned int v) { return __builtin_ctz(v); }
inline unsigned long long bsf(unsigned long long v) { return __builtin_ctzll(v); }
inline unsigned int bsr(unsigned int v) { return 31 - __builtin_clz(v); }
inline unsigned long long bsr(unsigned long long v) { return 63 - __builtin_clzll(v); }
inline unsigned int msb(unsigned int v) { return 1 << bsr(v); }
inline unsigned long long msb(unsigned long long v) { return 1ull << bsr(v); }
inline unsigned int lsb(unsigned int v) { return v & -v; }
inline unsigned long long lsb(unsigned long long v) { return v & -v; }
inline unsigned int pop_lsb(unsigned int v) { return v & v - 1; }
inline unsigned long long pop_lsb(unsigned long long v) { return v & v - 1; }
inline unsigned int round_up_pow2(unsigned int v) { return msb(v - 1) << 1; }
inline unsigned long long round_up_pow2(unsigned long long v) { return msb(v - 1) << 1; }


inline unsigned int next_bit_permutation(unsigned int x)
{
    int t = x | (x - 1);
    return (t + 1) | (unsigned int)((~t & -~t) - 1) >> (bsf(x) + 1);
}


inline unsigned long long next_bit_permutation(unsigned long long x)
{
    long t = x | (x - 1);
    return (t + 1) | (unsigned long long)((~t & -~t) - 1) >> (bsf(x) + 1);
}

template<class T>
T bit_swap(T x, size_t p1, size_t p2, size_t len)
{
    T mask = (static_cast<T>(1) << len) - 1;
    T ope = (x >> p1 ^ x >> p2) & mask; // 入れ替える場所のxorを求める
    return x ^ (ope << p1 | ope << p2); // 入れ替える場所をxor。
}

inline uint16_t get_lowest_bit(uint64_t num)
{
    if(num)
        return __builtin_ffsll(~num) - 1;
    return num;
}


#endif // BIT_MANIP
