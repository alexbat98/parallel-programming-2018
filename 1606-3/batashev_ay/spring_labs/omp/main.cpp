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
#include <omp.h>
#include <algorithm>
#include "merge.h"
#include "radix.h"


int main() {

    int64_t *data = nullptr;
    size_t n;

    std::cout << "Array size: ";
    std::cin >> n;

    std::default_random_engine generator(static_cast<int>(time(nullptr)));
    std::uniform_int_distribution<int64_t> distribution(-100000, 100000);

    data = new int64_t[n];

    for (size_t i = 0; i < n; i++) {
        data[i] = distribution(generator);
    }

    auto *sizes = new size_t[omp_get_num_procs()];
    auto *offsets = new size_t[omp_get_num_procs()];

    auto start = std::chrono::system_clock::now();
#pragma omp parallel shared(data, n, sizes)
    {
        int tid = omp_get_thread_num();
        int num_t = omp_get_num_threads();
        int offset = tid * n / num_t;
        int size = n / num_t;
        if (tid == num_t - 1) {
            size = n - offset;
        }
        sizes[tid] = size;
        offsets[tid] = offset;
        radix_sort(data + offset, size);
    }

    auto *isActive = new bool[omp_get_num_threads()];
    for (int i = 0; i < omp_get_num_threads(); ++i) {
        isActive[i] = i % 2 == 0;
    }

    for (int i = 0; i < omp_get_num_procs(); i++) {
        std::cout << "size[" << i << "]" << sizes[i] << "\n";
    }

#pragma omp parallel shared(sizes, data, n, offsets)
    {
        int factor = 2;
        int step = 1;
        for (int i = omp_get_num_procs() / 2; i > 1; i /= 2) {
            int tid = omp_get_thread_num();

            if (isActive[tid]) {

                if (tid == 0) {
                    std::cout << "Before merge\n";
                    std::cout << "offset " << offsets[tid];
                    std::cout << " size " << sizes[tid];
                    std::cout << " size+step " << sizes[tid + step] << "\n";
                    for (int k = offsets[tid]; k < offsets[tid] + sizes[tid] + sizes[tid + step];
                         k++) {
                        std::cout << data[k] << " ";
                    }
                    std::cout << "\n";
                }

                merge(
                    data,
                    offsets[tid],
                    offsets[tid] + sizes[tid] - 1,
                    offsets[tid] + sizes[tid] + sizes[tid + step] - 1
                );

                if (tid == 0) {
                    for (int idx = offsets[tid] + 1;
                         idx < offsets[tid] + sizes[tid] + sizes[tid + step]; idx++) {
                        if (data[idx - 1] > data[idx]) {
                            std::cout << "Merge failed in " << tid << " i = " << i << "\n";
                            std::cout << data[idx - 1] << ">" << data[idx] << "\n";
                            break;
                        }
                    }
                    std::cout << "After merge\n";
                    for (int k = offsets[tid]; k < offsets[tid] + sizes[tid] + sizes[tid + step];
                         k++) {
                        std::cout << data[k] << " ";
                    }
                    std::cout << "\n";
                }
            }
            for (int k = 0; k < omp_get_num_procs(); k++) {
                if (isActive[tid])
                    sizes[tid] += sizes[tid + step];
            }
            factor *= 2;
            for (int k = 0; k < omp_get_num_procs(); k++) {
                if (k % factor != 0) {
                    isActive[tid] = false;
                }
            }
            step *= 2;
#pragma omp barrier
            if (omp_get_thread_num() == 0) {
                std::cout << i << std::endl;
            }
        }
    }

    auto end = std::chrono::system_clock::now();

    std::cout << "Total time: "
              << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count()
              << "ms\n";

    bool isFail = false;
    for (int i = 1; i < n; i++) {
        if (data[i - 1] > data[i]) {
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