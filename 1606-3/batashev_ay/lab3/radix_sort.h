//
// Created by Александр Баташев on 28/11/2018.
//

#ifndef MPILAB3_RADIX_SORT_H
#define MPILAB3_RADIX_SORT_H
#include <cstdlib>
#ifdef INTEL
#include <cstdlib>
#include <xmmintrin.h>
#define SIMDPP_ARCH_X86_AVX2
#include "simdpp/simd.h"
#endif
void radix_sort(double *array, size_t size);
#endif //MPILAB3_RADIX_SORT_H
