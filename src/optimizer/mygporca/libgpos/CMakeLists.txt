project(duckdb_libgpos CXX C)

set(CMAKE_CXX_STANDARD "11" CACHE STRING "C++ standard to enforce")

include_directories(include)

include_directories(${PROJECT_SOURCE_DIR}/libgpopt/include/)

file(GLOB_RECURSE hdrs ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h)
file(GLOB_RECURSE srcs ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
if (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    file(GLOB_RECURSE srcs ${CMAKE_CURRENT_SOURCE_DIR}/src/dlfcn_win32.c)
endif()

cmake_policy(SET CMP0043 NEW)

SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DGPOS_DEBUG")

if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    add_compile_options(-DGPOS_Darwin)
endif()

add_compile_options(-DUSE_CMAKE)
# Turn on platform-specific defines.
set(GPOS_${CMAKE_SYSTEM_NAME} 1)
set(GPOS_${CMAKE_SYSTEM_PROCESSOR} 1)

# Add headers to make them visible in some IDEs (Clion, VS, Xcode)
list(APPEND srcs ${hdrs})

add_library(duckdb_libgpos STATIC ${srcs})

if(CMAKE_DL_LIBS)
    target_link_libraries(duckdb_libgpos ${CMAKE_DL_LIBS})
else()
#    target_link_libraries(duckdb_libgpos -ldl)
endif()

target_include_directories(
        duckdb_libgpos
        PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)
set_target_properties(duckdb_libgpos PROPERTIES EXPORT_NAME duckdb_libgpos)

install(TARGETS duckdb_libgpos
        EXPORT "${DUCKDB_EXPORT_SET}"
        LIBRARY DESTINATION "${INSTALL_LIB_DIR}"
        ARCHIVE DESTINATION "${INSTALL_LIB_DIR}")

disable_target_warnings(duckdb_libgpos)

#set_target_properties(duckdb_libgpos PROPERTIES POSITION_INDEPENDENT_CODE ON)
