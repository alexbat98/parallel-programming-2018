//
// Created by Александр Баташев on 2018-12-13.
//

#include "merge.h"
#include <vector>

void merge(std::vector<double> &where, double *arr, size_t middle, size_t end) {
    size_t lcur = 0;
    size_t rcur = 0;
    size_t cur = 0;

    while (lcur < middle && rcur < end) {
        if (arr[lcur] <= arr[middle + rcur]) {
            where[cur++] = arr[lcur++];
        } else {
            where[cur++] = arr[middle + rcur++];
        }
    }

    if (lcur < middle) {
        for (size_t i = lcur; i < middle; i++) {
            where[cur++] = arr[i];
        }
    } else if (rcur < end) {
        for (size_t i = rcur; i < end; i++) {
            where[cur++] = arr[middle + i];
        }
    }
}