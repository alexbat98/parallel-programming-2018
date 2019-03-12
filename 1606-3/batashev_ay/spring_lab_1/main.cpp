#include <iostream>
#include <cmath>
#include <vector>
#include <queue>
#include <random>
#include <iomanip>
#include <set>
#include <string>
#include <limits>
#include <chrono>

void radix_sort(uint64_t *data, size_t size) {

    std::vector<std::queue<uint64_t> > rad(65536);
    std::set<uint64_t> usedIdx;

    uint64_t mask = 65535;

    for (size_t r = 0; r < 4; r++) {

        for (size_t i = 0; i < size; i++) {
            uint64_t c = (data[i] & mask) >> (r * 16);
            rad[c].push(data[i]);
            usedIdx.insert(c);
        }

        mask = mask << 16;

        size_t idx = 0;

        for (uint64_t i : usedIdx) {
            while (!rad[i].empty()) {
                data[idx++] = rad[i].front();
                rad[i].pop();
            }
        }
        usedIdx.clear();
    }
}

int main() {

    uint64_t *data = nullptr;
    size_t n;

    std::cout << "Array size: ";
    std::cin >> n;

    std::default_random_engine generator(static_cast<unsigned int>(time(nullptr)));
    std::uniform_int_distribution<uint64_t> distribution(1, 100000000000);

    data = new uint64_t[n];

    for (size_t i = 0; i < n; i++) {
        data[i] = distribution(generator);
    }

    auto start = std::chrono::system_clock::now();
    radix_sort(data, n);
    auto end = std::chrono::system_clock::now();

    std::cout << "Total time: "
              << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count()
              << "ms\n";

    bool isFail = false;
    for (int i = 1; i < n; i++) {
        if (data[i-1] > data[i]) {
            isFail = true;
        }
    }

    if (isFail) {
        std::cout << "FAIL\n";
        for (size_t i = 0; i < n; i++) {
            std::cout << data[i] << " ";
        }
        std::cout << "\n";
    } else {
        std::cout << ".\n";
    }

    return 0;
}