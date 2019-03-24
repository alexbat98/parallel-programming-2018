add_executable(omp merge.cpp merge.h main.cpp radix.cpp radix.h)
add_executable(omp2 merge.cpp merge.h main2.cpp radix.cpp radix.h)
add_executable(omp3 merge.cpp merge.h main3.cpp radix.cpp radix.h)
add_executable(test_merge test.cpp merge.cpp merge.h)

find_package(OpenMP REQUIRED)
target_link_libraries(omp PUBLIC OpenMP::OpenMP_CXX)
target_link_libraries(omp2 PUBLIC OpenMP::OpenMP_CXX)
target_link_libraries(omp3 PUBLIC OpenMP::OpenMP_CXX)