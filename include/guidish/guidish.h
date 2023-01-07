/*
    GUIDISH - a 128bit guid like id that can be used as a monotonically increasing key
    64bits - epoch time in microseconds + 16 bits - siteid + 48 bits - cpu tick count
    useful for implementing message queues and similar id schemes.

    the site id field helps differentiate guidish keys generated in upto 65535 different sites
    per machine, per micro-second 2^48 - 1 unique keys can be generated on a single site

    the monotonical z-ordering is valid for the same site
    guidish(site, t1) > guidish(site, t2) if (t1 > t2)

    For x86/64 the tick count is obtained from the intrinsic __rtds()
    Optionally, std::chrono::high_resolution_clock can be used by defining GUIDISH_USE_CHRONO_FOR_TICKS


 */
#pragma once
#include <cstdint>
#include <functional>
#include <limits>
#include <iostream>
#include <iomanip>
#include <chrono>
#ifndef GUIDISH_USE_CHRONO_FOR_TICKS
#ifdef _WIN32
#include <intrin.h>
#pragma intrinsic (__rtds)
#else
#include <x86intrin.h>
#endif
#endif
namespace guidish {

namespace clock {
    uint64_t micros() {
        auto t = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch());
        return static_cast<uint64_t>(t.count());
    }
    uint64_t ticks() {
#ifndef GUIDISH_USE_CHRONO_FOR_TICKS
        static uint64_t kStart = __rdtsc();
        return __rdtsc() - kStart;
#else
        static auto kStart = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now());
        auto cur = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now());
        return static_cast<uint64_t>(cur - kStart);
#endif
    }
}

union id128_t {
    struct { uint64_t lo, hi; } nums;
    uint8_t octets[16];

    static const id128_t& max() {
        static const id128_t kVal (std::numeric_limits<uint64_t>::max(), std::numeric_limits<uint64_t>::max());
        return kVal;
    }

    static const id128_t& min() {
        static const id128_t kVal (std::numeric_limits<uint64_t>::min(), std::numeric_limits<uint64_t>::min());
        return kVal;
    }
    
    static id128_t gen() {
        return id128_t(clock::micros(), clock::ticks());
    }

    static id128_t gen(uint16_t siteID) {
        id128_t ret (clock::micros(), clock::ticks());
        ret.nums.lo |= (static_cast<uint64_t>(siteID) << 48);
        return ret;
    }
    id128_t(uint64_t hi, uint64_t lo) : nums{lo, hi} {}
    id128_t(uint64_t lo) : nums {lo, 0} {}
    id128_t() : nums {0, 0} {}
    id128_t(const id128_t& id) {
        *this = id;
    }
    const id128_t& operator = (const id128_t& id) {
        nums.lo = id.nums.lo;
        nums.hi = id.nums.hi;
        return *this;
    }

    void operator ++ () {
        incr();
    }
 
    void operator -- () {
        decr();
    }
 
    id128_t operator ++ (int) {
        id128_t tmp = *this;
        tmp.incr();
        return tmp;
    }
 
    id128_t operator -- (int) {
        id128_t tmp = *this;
        tmp.decr();
        return tmp;
    }

    const id128_t& incr() {
        if (nums.lo == std::numeric_limits<uint64_t>::max()) {
            nums.lo = std::numeric_limits<uint64_t>::min();
            nums.hi += 1;
        } else {
            nums.lo += 1;
        }
        return *this;
    }
 
    const id128_t& decr() {
        if (nums.lo == std::numeric_limits<uint64_t>::min()) {
            nums.lo = std::numeric_limits<uint64_t>::max();
            nums.hi -= 1;
        } else {
            nums.lo -= 1;
        }
        return *this;
    }

    const id128_t& flip() {
        for (auto i = 0; i < 8; i++) {
            std::swap<uint8_t>(octets[i], octets[15 - i]);
        }
        return *this;
    }

    bool equals(const id128_t& id) const {
        return nums.hi == id.nums.hi && nums.lo == id.nums.lo;
    }

    bool lt(const id128_t& id) const {
        if (nums.hi < id.nums.hi) { return true; }
        else if (nums.hi > id.nums.hi) { return false; }
        else { return nums.lo < id.nums.lo; }
    }

    bool gt(const id128_t& id) const {
        if (nums.hi > id.nums.hi) { return true; }
        else if (nums.hi < id.nums.hi) { return false; }
        else { return nums.lo > id.nums.lo; }
    }

    std::ostream& print(std::ostream& os, bool guidHyphens = true) const {
        for (size_t idx = 16; idx > 0; idx--) {
            if (guidHyphens && (idx == 4 || idx == 6 || idx == 8 || idx == 10)) {
                os << "-";
            }
            os << std::hex << std::setw(2) << std::setfill('0') << static_cast<uint16_t>(octets[idx - 1]) << std::dec;
        }
        return os;
    }
    
    std::string toString() const {
        std::ostringstream os;
        print(os);
        return os.str();
    }
};
};

inline bool operator == (const guidish::id128_t& a, const guidish::id128_t& b) {
    return a.equals(b);
}

inline bool operator < (const guidish::id128_t& a, const guidish::id128_t& b) {
    return a.lt(b);
}

inline bool operator > (const guidish::id128_t& a, const guidish::id128_t& b) {
    return a.gt(b);
}

inline std::ostream& operator << (std::ostream& os, const guidish::id128_t& id) {
    return id.print(os);
}


namespace std {
template <>
struct hash<guidish::id128_t> {
    size_t operator()(const guidish::id128_t& id) const {
        return (std::hash<uint64_t>()(id.nums.hi) << 1) ^ (std::hash<uint64_t>()(id.nums.lo));
    }
};
template <>
struct equal_to<guidish::id128_t> {
    bool operator()(const guidish::id128_t& a, const guidish::id128_t& b) const {
        return a.equals(b);
    }
};
template <>
struct less<guidish::id128_t> {
    bool operator()(const guidish::id128_t& a, const guidish::id128_t& b) const {
        return a.lt(b);
    }
};
template <>
struct greater<guidish::id128_t> {
    bool operator()(const guidish::id128_t& a, const guidish::id128_t& b) const {
        return a.gt(b);
    }
};
}

namespace guidish {
class Gen {
public:
    Gen(uint16_t site) : site_(site) {}
    virtual ~Gen() = default;
    id128_t operator()() const {
        return id128_t::gen(site_);
    }
private:
    uint16_t site_;
};
}