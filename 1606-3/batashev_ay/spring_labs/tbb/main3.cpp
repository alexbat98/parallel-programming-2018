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
#include <chrono>

#include "tbb/task.h"
#include "tbb/task_scheduler_init.h"
#include "tbb/task_group.h"
#include "tbb/parallel_invoke.h"


using namespace tbb;

class SortTask {
private:
    int64_t *mData;
    size_t mSize;
public:
    SortTask(int64_t *data, size_t size) : mData(data), mSize(size) {}
    SortTask(const SortTask& src) {
        mData = src.mData;
        mSize = src.mSize;
    }

    SortTask(SortTask&& src) noexcept {
        mData = src.mData;
        mSize = src.mSize;
        src.mSize = 0;
        src.mData = nullptr;
    }

    SortTask operator()() const {
        radix_sort(mData, mSize);
        return *this;
    }

    ~SortTask() = default;
};

void merge_tbb (int64_t *a, int *sizes, int k)
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

    parallel_invoke(
        [=]{merge_tbb(a, sizes, k / 2);},
        [=]{merge_tbb(a + offset, sizes + k / 2, k / 2);}
        );

	merge2(a, offset - 1, total - 1);
}

int main() {

    task_scheduler_init init;

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

    int chunk = n / task_scheduler_init::default_num_threads();
    int max_chunk = chunk + (n - chunk * task_scheduler_init::default_num_threads());

    //std::cout << "Max chunk " << max_chunk << "\n";

    auto *sizes = new int[task_scheduler_init::default_num_threads()];

    auto start = std::chrono::system_clock::now();

    task_group sort_tasks;

    for (int t = 0; t < task_scheduler_init::default_num_threads(); t++) {
        int offset = t * n / task_scheduler_init::default_num_threads();
        int size = n / task_scheduler_init::default_num_threads();
        if (t == task_scheduler_init::default_num_threads() - 1) {
            size = n - offset;
        }
        sizes[t] = size;

        SortTask task(data+offset, size);
        sort_tasks.run(task);
    }

    sort_tasks.wait();

//    {
//        int tid = omp_get_thread_num();
//        int num_t = omp_get_num_threads();
//        int offset = tid * n / num_t;
//        int size = n / num_t;
//        if (tid == num_t - 1) {
//            size = n - offset;
//        }
//        sizes[tid] = size;
//        radix_sort(data + offset, size);
//    }
    auto end = std::chrono::system_clock::now();

    auto sort = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    start = std::chrono::system_clock::now();
    merge_tbb(data, sizes, task_scheduler_init::default_num_threads());
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
