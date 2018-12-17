cmake_minimum_required(VERSION 3.9)
project(mpilab3)

set(CMAKE_CXX_STANDARD 17)

find_package(MPI REQUIRED)

option(INTEL OFF)

if (INTEL)
    set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-O3 -fno-rtti -fno-exceptions -mavx2 -mtune=skylake -unroll -qopt-mem-layout-trans -use-intel-optimized-headers -qopt-prefetch=3 -qopt-prefetch-distance=32000")
    add_definitions(-DINTEL)
#    add_definitions(-DL1CACHE_SIZE=32000)
#    add_definitions(-DL2CACHE_SIZE=256000)
#    add_definitions(-DL2_PREF_SIZE=32000)
#    add_definitions(-DL1_PREF_SIZE=4000)
#    add_definitions(-DOPTIMAL_THREADS=4)
endif()
#set(CMAKE_CXX_FLAGS "-qopt-report=5 -simd -msse4.2")

add_executable(mpilab3 main.cpp radix_sort.cpp merge.cpp merge.h)
add_executable(mpilab3par parmerge.cpp radix_sort.cpp merge.cpp)
add_executable(tests tests.cpp)

include_directories(SYSTEM ${MPI_INCLUDE_PATH})
include_directories(${CMAKE_SOURCE_DIR})
target_link_libraries(mpilab3 PUBLIC ${MPI_CXX_LIBRARIES})
target_link_libraries(mpilab3par PUBLIC ${MPI_CXX_LIBRARIES})

message(${MPI_INCLUDE_PATH})
message(${MPI_CXX_LIBRARIES})

find_package(Threads)
target_link_libraries(mpilab3 PUBLIC ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(mpilab3par PUBLIC ${CMAKE_THREAD_LIBS_INIT})
target_compile_definitions(mpilab3par PUBLIC "$<$<CONFIG:DEBUG>:DEBUG>")
