/*
 * Copyright (c) 2019 Alexander Batashev. All rights reserved.
 *
 * Licensed under MIT license.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an “AS IS” BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 * License for the specific language governing permissions and limitations under
 * the License.
 */

#include <cstdint>
#include <iostream>
#include <random>
#include "radix.h"
#include "merge.h"
#include "omp.h"
#include <chrono>

void merge_omp (int64_t *a, int *sizes, int k)
{
    if (k == 2) {
        //merge(a, 0, sizes[0] - 1, sizes[0]+sizes[1] - 1);
		merge2(a, sizes[0] - 1, sizes[0] + sizes[1] - 1);
        return;
    }

    int offset = 0;
    for (int i = 0; i < k / 2; i++) {
        offset += sizes[i];
    }
    int total = offset;
    for (int i = k / 2; i < k; i++) {
        total += sizes[i];
    }

    int priority = omp_get_num_procs() - k;

#pragma omp task priority(priority) firstprivate(a, sizes, k)
    merge_omp(a, sizes, k / 2);

#pragma omp task priority(priority) firstprivate(a, sizes, k)
    merge_omp(a + offset, sizes + k / 2, k / 2);

#pragma omp taskwait
	merge2(a, offset - 1, total - 1);
    //merge(a, 0, offset - 1, total - 1);
}

int main() {

    int64_t *data = nullptr;
    size_t n = 0;

    std::cout << "Array size: ";
    std::cin >> n;

    std::default_random_engine generator(static_cast<int>(time(nullptr)));
    std::uniform_int_distribution<int64_t> distribution(-100000, 100000);

    data = new int64_t[n];

    for (size_t i = 0; i < n; i++) {
        data[i] = distribution(generator);
    }

    int chunk = n / omp_get_num_procs();
    int max_chunk = chunk + (n - chunk * omp_get_num_procs());

    std::cout << "Max chunk " << max_chunk << "\n";

    auto *sizes = new int[omp_get_num_procs()];

    auto start = std::chrono::system_clock::now();

#pragma omp parallel firstprivate(data, n)
    {
        int tid = omp_get_thread_num();
        int num_t = omp_get_num_threads();
        int offset = tid * n / num_t;
        int size = n / num_t;
        if (tid == num_t - 1) {
            size = n - offset;
        }
        sizes[tid] = size;
        radix_sort(data + offset, size);
    }
    auto end = std::chrono::system_clock::now();

    auto sort = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    start = std::chrono::system_clock::now();
    merge_omp(data, sizes, omp_get_num_procs());
    end = std::chrono::system_clock::now();

    auto merge = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();


    std::cout << "Sort time: "
              << sort
              << "ms\n";

    std::cout << "Merge time: "
              << merge
              << "ms\n";

    std::cout << "Total time: "
              << sort + merge
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
