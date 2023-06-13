

function(conan_message MESSAGE_OUTPUT)
    if(NOT CONAN_CMAKE_SILENT_OUTPUT)
        message(${ARGV${0}})
    endif()
endfunction()


macro(conan_find_apple_frameworks FRAMEWORKS_FOUND FRAMEWORKS FRAMEWORKS_DIRS)
    if(APPLE)
        foreach(_FRAMEWORK ${FRAMEWORKS})
            # https://cmake.org/pipermail/cmake-developers/2017-August/030199.html
            find_library(CONAN_FRAMEWORK_${_FRAMEWORK}_FOUND NAMES ${_FRAMEWORK} PATHS ${FRAMEWORKS_DIRS} CMAKE_FIND_ROOT_PATH_BOTH)
            if(CONAN_FRAMEWORK_${_FRAMEWORK}_FOUND)
                list(APPEND ${FRAMEWORKS_FOUND} ${CONAN_FRAMEWORK_${_FRAMEWORK}_FOUND})
            else()
                message(FATAL_ERROR "Framework library ${_FRAMEWORK} not found in paths: ${FRAMEWORKS_DIRS}")
            endif()
        endforeach()
    endif()
endmacro()


function(conan_package_library_targets libraries package_libdir deps out_libraries out_libraries_target build_type package_name)
    unset(_CONAN_ACTUAL_TARGETS CACHE)
    unset(_CONAN_FOUND_SYSTEM_LIBS CACHE)
    foreach(_LIBRARY_NAME ${libraries})
        find_library(CONAN_FOUND_LIBRARY NAMES ${_LIBRARY_NAME} PATHS ${package_libdir}
                     NO_DEFAULT_PATH NO_CMAKE_FIND_ROOT_PATH)
        if(CONAN_FOUND_LIBRARY)
            conan_message(STATUS "Library ${_LIBRARY_NAME} found ${CONAN_FOUND_LIBRARY}")
            list(APPEND _out_libraries ${CONAN_FOUND_LIBRARY})
            if(NOT ${CMAKE_VERSION} VERSION_LESS "3.0")
                # Create a micro-target for each lib/a found
                string(REGEX REPLACE "[^A-Za-z0-9.+_-]" "_" _LIBRARY_NAME ${_LIBRARY_NAME})
                set(_LIB_NAME CONAN_LIB::${package_name}_${_LIBRARY_NAME}${build_type})
                if(NOT TARGET ${_LIB_NAME})
                    # Create a micro-target for each lib/a found
                    add_library(${_LIB_NAME} UNKNOWN IMPORTED)
                    set_target_properties(${_LIB_NAME} PROPERTIES IMPORTED_LOCATION ${CONAN_FOUND_LIBRARY})
                    set(_CONAN_ACTUAL_TARGETS ${_CONAN_ACTUAL_TARGETS} ${_LIB_NAME})
                else()
                    conan_message(STATUS "Skipping already existing target: ${_LIB_NAME}")
                endif()
                list(APPEND _out_libraries_target ${_LIB_NAME})
            endif()
            conan_message(STATUS "Found: ${CONAN_FOUND_LIBRARY}")
        else()
            conan_message(STATUS "Library ${_LIBRARY_NAME} not found in package, might be system one")
            list(APPEND _out_libraries_target ${_LIBRARY_NAME})
            list(APPEND _out_libraries ${_LIBRARY_NAME})
            set(_CONAN_FOUND_SYSTEM_LIBS "${_CONAN_FOUND_SYSTEM_LIBS};${_LIBRARY_NAME}")
        endif()
        unset(CONAN_FOUND_LIBRARY CACHE)
    endforeach()

    if(NOT ${CMAKE_VERSION} VERSION_LESS "3.0")
        # Add all dependencies to all targets
        string(REPLACE " " ";" deps_list "${deps}")
        foreach(_CONAN_ACTUAL_TARGET ${_CONAN_ACTUAL_TARGETS})
            set_property(TARGET ${_CONAN_ACTUAL_TARGET} PROPERTY INTERFACE_LINK_LIBRARIES "${_CONAN_FOUND_SYSTEM_LIBS};${deps_list}")
        endforeach()
    endif()

    set(${out_libraries} ${_out_libraries} PARENT_SCOPE)
    set(${out_libraries_target} ${_out_libraries_target} PARENT_SCOPE)
endfunction()


include(FindPackageHandleStandardArgs)

conan_message(STATUS "Conan: Using autogenerated FindCatch2.cmake")
# Global approach
set(Catch2_FOUND 1)
set(Catch2_VERSION "2.13.4")

find_package_handle_standard_args(Catch2 REQUIRED_VARS
                                  Catch2_VERSION VERSION_VAR Catch2_VERSION)
mark_as_advanced(Catch2_FOUND Catch2_VERSION)


set(Catch2_INCLUDE_DIRS "/home/aroonsak/.conan/data/catch2/2.13.4/_/_/package/5ab84d6acfe1f23c4fae0ab88f26e3a396351ac9/include")
set(Catch2_INCLUDE_DIR "/home/aroonsak/.conan/data/catch2/2.13.4/_/_/package/5ab84d6acfe1f23c4fae0ab88f26e3a396351ac9/include")
set(Catch2_INCLUDES "/home/aroonsak/.conan/data/catch2/2.13.4/_/_/package/5ab84d6acfe1f23c4fae0ab88f26e3a396351ac9/include")
set(Catch2_RES_DIRS )
set(Catch2_DEFINITIONS )
set(Catch2_LINKER_FLAGS_LIST
        "$<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,SHARED_LIBRARY>:>"
        "$<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,MODULE_LIBRARY>:>"
        "$<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,EXECUTABLE>:>"
)
set(Catch2_COMPILE_DEFINITIONS )
set(Catch2_COMPILE_OPTIONS_LIST "" "")
set(Catch2_COMPILE_OPTIONS_C "")
set(Catch2_COMPILE_OPTIONS_CXX "")
set(Catch2_LIBRARIES_TARGETS "") # Will be filled later, if CMake 3
set(Catch2_LIBRARIES "") # Will be filled later
set(Catch2_LIBS "") # Same as Catch2_LIBRARIES
set(Catch2_SYSTEM_LIBS )
set(Catch2_FRAMEWORK_DIRS )
set(Catch2_FRAMEWORKS )
set(Catch2_FRAMEWORKS_FOUND "") # Will be filled later
set(Catch2_BUILD_MODULES_PATHS )

conan_find_apple_frameworks(Catch2_FRAMEWORKS_FOUND "${Catch2_FRAMEWORKS}" "${Catch2_FRAMEWORK_DIRS}")

mark_as_advanced(Catch2_INCLUDE_DIRS
                 Catch2_INCLUDE_DIR
                 Catch2_INCLUDES
                 Catch2_DEFINITIONS
                 Catch2_LINKER_FLAGS_LIST
                 Catch2_COMPILE_DEFINITIONS
                 Catch2_COMPILE_OPTIONS_LIST
                 Catch2_LIBRARIES
                 Catch2_LIBS
                 Catch2_LIBRARIES_TARGETS)

# Find the real .lib/.a and add them to Catch2_LIBS and Catch2_LIBRARY_LIST
set(Catch2_LIBRARY_LIST )
set(Catch2_LIB_DIRS "/home/aroonsak/.conan/data/catch2/2.13.4/_/_/package/5ab84d6acfe1f23c4fae0ab88f26e3a396351ac9/lib")

# Gather all the libraries that should be linked to the targets (do not touch existing variables):
set(_Catch2_DEPENDENCIES "${Catch2_FRAMEWORKS_FOUND} ${Catch2_SYSTEM_LIBS} ")

conan_package_library_targets("${Catch2_LIBRARY_LIST}"  # libraries
                              "${Catch2_LIB_DIRS}"      # package_libdir
                              "${_Catch2_DEPENDENCIES}"  # deps
                              Catch2_LIBRARIES            # out_libraries
                              Catch2_LIBRARIES_TARGETS    # out_libraries_targets
                              ""                          # build_type
                              "Catch2")                                      # package_name

set(Catch2_LIBS ${Catch2_LIBRARIES})

foreach(_FRAMEWORK ${Catch2_FRAMEWORKS_FOUND})
    list(APPEND Catch2_LIBRARIES_TARGETS ${_FRAMEWORK})
    list(APPEND Catch2_LIBRARIES ${_FRAMEWORK})
endforeach()

foreach(_SYSTEM_LIB ${Catch2_SYSTEM_LIBS})
    list(APPEND Catch2_LIBRARIES_TARGETS ${_SYSTEM_LIB})
    list(APPEND Catch2_LIBRARIES ${_SYSTEM_LIB})
endforeach()

# We need to add our requirements too
set(Catch2_LIBRARIES_TARGETS "${Catch2_LIBRARIES_TARGETS};")
set(Catch2_LIBRARIES "${Catch2_LIBRARIES};")

set(CMAKE_MODULE_PATH "/home/aroonsak/.conan/data/catch2/2.13.4/_/_/package/5ab84d6acfe1f23c4fae0ab88f26e3a396351ac9/lib/cmake/Catch2" ${CMAKE_MODULE_PATH})
set(CMAKE_PREFIX_PATH "/home/aroonsak/.conan/data/catch2/2.13.4/_/_/package/5ab84d6acfe1f23c4fae0ab88f26e3a396351ac9/lib/cmake/Catch2" ${CMAKE_PREFIX_PATH})

if(NOT ${CMAKE_VERSION} VERSION_LESS "3.0")
    # Target approach
    if(NOT TARGET Catch2::Catch2)
        add_library(Catch2::Catch2 INTERFACE IMPORTED)
        if(Catch2_INCLUDE_DIRS)
            set_target_properties(Catch2::Catch2 PROPERTIES INTERFACE_INCLUDE_DIRECTORIES
                                  "${Catch2_INCLUDE_DIRS}")
        endif()
        set_property(TARGET Catch2::Catch2 PROPERTY INTERFACE_LINK_LIBRARIES
                     "${Catch2_LIBRARIES_TARGETS};${Catch2_LINKER_FLAGS_LIST}")
        set_property(TARGET Catch2::Catch2 PROPERTY INTERFACE_COMPILE_DEFINITIONS
                     ${Catch2_COMPILE_DEFINITIONS})
        set_property(TARGET Catch2::Catch2 PROPERTY INTERFACE_COMPILE_OPTIONS
                     "${Catch2_COMPILE_OPTIONS_LIST}")
        
    endif()
endif()

foreach(_BUILD_MODULE_PATH ${Catch2_BUILD_MODULES_PATHS})
    include(${_BUILD_MODULE_PATH})
endforeach()
