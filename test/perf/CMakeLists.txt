add_subdirectory(googlebenchmark)

include_directories(${PROJECT_SOURCE_DIR}/include googlebenchmark/include)

add_executable(looping looping.cpp ${PROJECT_SOURCE_DIR}/include/AVArray/AVArray.hpp googlebenchmark/include/benchmark/benchmark.h)

link_directories(googlebenchmark)
target_link_libraries(looping benchmark)

add_custom_target(perftest DEPENDS benchmark looping)
add_custom_command(TARGET perftest POST_BUILD COMMAND looping)