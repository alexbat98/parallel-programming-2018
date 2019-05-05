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


void merge2(int64_t *arr, int middle, int end) {
	auto temp = new int64_t[end + 1];
	int start2 = middle + 1;
	int start = 0;
	int k = 0;

	while (start <= middle && start2 <= end) {
		if (arr[start] < arr[start2]) {
			temp[k++] = arr[start++];
		} else {
			temp[k++] = arr[start2++];
		}
	}

	while (start <= middle) {
        temp[k++] = arr[start++];
	}

	while (start2 <= end) {
        temp[k++] = arr[start2++];
	}

	for (int i = 0; i <= end; i++) {
		arr[i] = temp[i];
	}

	delete[] temp;
}

// Merges two subarrays of arr[].
// First subarray is arr[l..m]
// Second subarray is arr[m+1..r]
// Inplace Implmentation
void merge(int64_t *arr, int start, int mid, int end) {
    int start2 = mid + 1;

    // If the direct merge is already sorted
    if (arr[mid] <= arr[start2]) {
        return;
    }

    // Two pointers to maintain start
    // of both arrays to merge
    while (start <= mid && start2 <= end) {

        // If element 1 is in right place
        if (arr[start] <= arr[start2]) {
            start++;
        } else {
            int value = arr[start2];
            int index = start2;

            // Shift all the elements between element 1
            // element 2, right by 1.
            while (index != start) {
                arr[index] = arr[index - 1];
                index--;
            }
            arr[start] = value;

            // Update all the pointers
            start++;
            mid++;
            start2++;
        }
    }
}