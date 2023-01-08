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

    ----------------------------------------------------------------
    Github: https://github.com/mohaps/guidish/
    Author: mohaps AT gmail DOT com
    ----------------------------------------------------------------

    Copyright 2023 Saurav Mohapatra (mohaps@gmail.com)

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
 */
#include "guidish/guidish.h"
#include <unordered_set>
using namespace guidish;

int main(int argc, char** argv) {
    try {
        id128_t id;
        std::cout << id << std::endl;
        id = id128_t::gen(0xbeef);
        std::cout << id << std::endl;
        std::cout << id.flip() << std::endl;

        size_t count = 1000ULL * 1000ULL;
        id128_t prev;
        Gen gen(0xabcd);
        uint64_t start = guidish::clock::micros();
        for (size_t i = 0; i < count; i++) {
            id128_t id = gen();
            if (id == prev) {
                std::cerr << "id collision: "<< id << std::endl;
                return -1;
            } 
            if (id < prev) {
                std::cerr << "id less: "<<id<<" LT " << prev << std::endl;
                return -1;
            }
            prev = id;
            if ((i+1) % 200000== 0) {
                uint64_t elapsed = guidish::clock::micros() - start;
                std::cout << "progress: done ["<<i+1<<"] ids in "<< elapsed << " usecs => " << id << std::endl;
            }
        }
        uint64_t us = guidish::clock::micros() - start;
        std::cout << "success: generated " << count << " monotonically increasing ids without collision in "<< us <<" usecs!" << std::endl;
    } catch (std::exception& ex) {
        std::cerr << "error: " << ex.what() << std::endl;
        return -1;
    }
}