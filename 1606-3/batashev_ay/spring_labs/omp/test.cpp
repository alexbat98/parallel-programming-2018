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
#include "merge.h"

int main() {
    int64_t arr[] = {-28036, 55308, 31714, 40971 };

    merge(arr, 0, 1, 3);

    for (int i = 1; i < 4; i++) {
        if (arr[i-1] > arr[i]) {
            return 1;
        }
    }
    return 0;
}