find_package(fmt REQUIRED)

if(${BP_BUILD_TESTS_COVERAGE})
    target_compile_options(cmake_cpp_boilerplate_lib PUBLIC -fprofile-arcs -ftest-coverage)
    target_link_libraries(cmake_cpp_boilerplate_lib PUBLIC gcov)
endif()

add_executable(AProcessManager AProcessManager.cpp)
add_executable(ECommandServer ECommandServer.cpp)
add_executable(EAutomation EAutomation.cpp)
add_executable(EBroadcast EBroadcast.cpp)
add_executable(ECollector ECollector.cpp)
add_executable(EFileMan EFileMan.cpp)
add_executable(ELogger ELogger.cpp)
add_executable(EMonitor EMonitoring.cpp)

target_link_libraries(ECommandServer rt)