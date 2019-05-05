add_executable(tbb merge.cpp merge.h main3.cpp radix.cpp radix.h)

if (NOT WIN32)
    include_directories(/opt/intel/tbb/include)
    link_directories(/opt/intel/tbb/lib)
endif()

find_library(tbb tbb)

target_link_libraries(tbb ${tbb})