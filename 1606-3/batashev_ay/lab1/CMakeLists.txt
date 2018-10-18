cmake_minimum_required(VERSION 3.9)
project(mpilab1)

set(CMAKE_CXX_STANDARD 17)

find_package(MPI REQUIRED)

add_executable(mpilab1 main.cpp)
include_directories(SYSTEM ${MPI_INCLUDE_PATH})
target_link_libraries(mpilab1_v1 PUBLIC ${MPI_CXX_LIBRARIES})
