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