include_directories(include)

# Generate version-number header.
# for the generated config.h file.
include_directories(${PROJECT_SOURCE_DIR}/src/include/duckdb/)
include_directories(${PROJECT_SOURCE_DIR}/src/include/duckdb/optimizer/mygporca/libgpos/include/)

file(GLOB_RECURSE hdrs ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h)
file(GLOB_RECURSE srcs ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)

# Add headers to make them visible in some IDEs (Clion, VS, Xcode)
list(APPEND srcs ${hdrs})

add_library(orca_optimizer STATIC ${srcs})

target_link_libraries(orca_optimizer
                      ${CMAKE_DL_LIBS}
                      gpos)
