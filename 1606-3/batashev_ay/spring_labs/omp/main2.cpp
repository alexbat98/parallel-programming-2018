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

void mergesort_parallel_omp (int64_t *a, int size, int size_threshold)
{
    if (size <= size_threshold) {
        radix_sort(a, size);
        return;
    }


#pragma omp parallel sections
    {
#pragma omp section
        {
            mergesort_parallel_omp(a, size / 2, size_threshold);
        }

#pragma omp section
        {
            mergesort_parallel_omp(a + size / 2, size - size / 2, size_threshold);
        }
    }

    merge(a, 0, size/2 - 1, size - 1);
}

// span parallel region outside once outside
void sort_omp(int64_t *a, int size, int size_threshold) {
    mergesort_parallel_omp(a, size, size_threshold);
}


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

    int chunk = n / omp_get_num_procs();
    int max_chunk = chunk + (n - chunk * omp_get_num_procs());

    std::cout << "Max chunk " << max_chunk << "\n";

    auto start = std::chrono::system_clock::now();
    sort_omp(data, n, max_chunk*4);
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