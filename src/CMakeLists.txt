find_package(fmt REQUIRED)

if(${BP_BUILD_TESTS_COVERAGE})
    target_compile_options(cmake_cpp_boilerplate_lib PUBLIC -fprofile-arcs -ftest-coverage)
    target_link_libraries(cmake_cpp_boilerplate_lib PUBLIC gcov)
endif()

add_executable(IMU AIMU.cpp) # gcc -o GNC -lfmt -lrt -lpthread -lwiringPi IMU.cpp
target_link_libraries(IMU fmt)
target_link_libraries(IMU rt)
target_link_libraries(IMU pthread)
target_link_libraries(IMU wiringPi)
# target_link_libraries(IMU wiringPi)
