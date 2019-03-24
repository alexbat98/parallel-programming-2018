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
#include "radix.h"
#include <iostream>
#include <cmath>
#include <vector>
#include <queue>
#include <set>
#include <limits>
#include <algorithm>

void radix_sort(int64_t *data, size_t size) {

    std::vector<std::queue<int64_t>> rad(65536);
    std::set<uint64_t> usedIdx;
    uint64_t mask = 65535;

    for (size_t r = 0; r < 4; r++) {

        for (size_t i = 0; i < size; i++) {
            uint64_t c = (data[i] & mask) >> (r * 16);
            if (r == 3) {
                c ^= 1UL << 15UL;
            }
            rad[c].push(data[i]);
            usedIdx.insert(c);
        }

        mask = mask << 16U;

        size_t idx = 0;

        for (int i = 0; i < 65536; i++) {
            while (!rad[i].empty()) {
                data[idx++] = rad[i].front();
                rad[i].pop();
            }
        }
        usedIdx.clear();
    }
}