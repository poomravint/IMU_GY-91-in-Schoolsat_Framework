find_package(fmt REQUIRED)

# add_library(cmake_cpp_boilerplate_lib
#     "../include/dummy.hpp"
#     dummy.cpp)
# add_library(cmake_cpp_boilerplate::lib ALIAS cmake_cpp_boilerplate_lib)
# target_link_libraries(cmake_cpp_boilerplate_lib
#         PUBLIC cmake_cpp_boilerplate::compiler_options fmt::fmt)
# target_include_directories(cmake_cpp_boilerplate_lib PUBLIC "${PROJECT_SOURCE_DIR}/include")

if(${BP_BUILD_TESTS_COVERAGE})
    target_compile_options(cmake_cpp_boilerplate_lib PUBLIC -fprofile-arcs -ftest-coverage)
    target_link_libraries(cmake_cpp_boilerplate_lib PUBLIC gcov)
endif()

add_executable(AGNC AGNC.cpp) # gcc -o GNC -lfmt -lrt -lpthread -lwiringPi AGNC.cpp
target_link_libraries(AGNC fmt)
target_link_libraries(AGNC rt)
target_link_libraries(AGNC pthread)
# target_link_libraries(AGNC wiringPi)

# add_executable(cmake_cpp_boilerplate_app "main.cpp")
# target_link_libraries(cmake_cpp_boilerplate_app PRIVATE cmake_cpp_boilerplate::lib)

# add_executable(SHMW "shared_memory_w.cpp")
# target_link_libraries(SHMW rt)
# target_link_libraries(SHMW pthread)
# add_executable(SHMR "shared_memory_r.cpp")
# target_link_libraries(SHMR rt)
# target_link_libraries(SHMR pthread)

# add_executable(Test1 "test1.cpp")
# add_executable(Test2 "test2.cpp")
# target_link_libraries(cmake_cpp_boilerplate_app PRIVATE cmake_cpp_boilerplate::lib)
