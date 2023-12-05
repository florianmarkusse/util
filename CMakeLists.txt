cmake_minimum_required(VERSION 3.18)
project(util)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

### Options ###
option(BUILD_SHARED_LIBS "Build shared libraries instead of static libraries" OFF)
option(CMAKE_BUILD_TYPE "Build type (Debug, Release, Profiling)" "Release")

### Flags ###
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Wconversion -Wno-sign-conversion -Wno-missing-field-initializers -Wdouble-promotion -Wvla -W")

if (CMAKE_BUILD_TYPE STREQUAL "Profiling")
    SET(CMAKE_C_FLAGS "${CMAKE_CXX_FLAGS} -pg")
    SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -pg")
    SET(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -pg")
endif()

set(FLO_UTIL_LIBRARY "${PROJECT_NAME}-${CMAKE_BUILD_TYPE}")

if(BUILD_SHARED_LIBS)
    set(LIBRARY_TYPE "SHARED")
else()
    set(LIBRARY_TYPE "STATIC")
endif()
add_library(${FLO_UTIL_LIBRARY} ${LIBRARY_TYPE}
    "src/file/read.c"
    "src/file/path.c"
    "src/hash/msi/common.c"
    "src/hash/msi/string-set.c"
    "src/hash/msi/uint16-set.c"
    "src/hash/trie/string-set.c"
    "src/hash/trie/string-uint16-map.c"
    "src/hash/trie/uint16-set.c"
    "src/hash/hash-comparison-status.c"
    "src/memory/arena.c"
    "src/memory/buddy-allocator.c"
    "src/memory/pool-allocator.c"
    "src/log.c"
)

target_compile_options(${FLO_UTIL_LIBRARY} PRIVATE
    $<$<CONFIG:Debug>:-O0 -g3>
    $<$<CONFIG:Release>:-O3>
    $<$<CONFIG:Profiling>:-O2 -pg>
)

target_compile_definitions(${FLO_UTIL_LIBRARY} PRIVATE
    $<$<CONFIG:Debug>:DEBUG>
    $<$<CONFIG:Release>:>
    $<$<CONFIG:Profiling>:>
)

set(FLO_UTIL_INCLUDES
    ${CMAKE_CURRENT_SOURCE_DIR}/include/
)
target_include_directories(${FLO_UTIL_LIBRARY} PRIVATE ${FLO_UTIL_INCLUDES})

# Install the library
install(TARGETS ${FLO_UTIL_LIBRARY}
        DESTINATION lib
        EXPORT ${PROJECT_NAME}Config)

# Install header files
install(DIRECTORY include/ DESTINATION include)

# Generate a Config.cmake file
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
    VERSION 1.0.0
    COMPATIBILITY AnyNewerVersion
)

configure_package_config_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/${PROJECT_NAME}Config.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake
    INSTALL_DESTINATION lib/cmake/${PROJECT_NAME}
)

# Install the Config.cmake files
install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
    DESTINATION lib/cmake/${PROJECT_NAME}
)

# Configure CPack for creating a package
set(CPACK_PACKAGE_NAME ${PROJECT_NAME})
set(CPACK_PACKAGE_VERSION "1.0.0")  # Set your version here
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Utility functions for C")
set(CPACK_PACKAGE_VENDOR "Florian Markusse")
set(CPACK_PACKAGE_CONTACT "florianmarkusse@gotmail.com")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
set(CPACK_RESOURCE_FILE_README "${CMAKE_CURRENT_SOURCE_DIR}/README.md")
set(CPACK_GENERATOR "ZIP") 

include(CPack)
