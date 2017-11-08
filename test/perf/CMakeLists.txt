add_subdirectory(googlebenchmark)

include_directories(${PROJECT_SOURCE_DIR}/include googlebenchmark/include)

add_executable(looping looping.cpp ${PROJECT_SOURCE_DIR}/include/AVArray/AVArray.hpp googlebenchmark/include/benchmark/benchmark.h)
add_executable(matmult matmult.cpp ${PROJECT_SOURCE_DIR}/include/AVArray/AVArray.hpp googlebenchmark/include/benchmark/benchmark.h)

target_compile_options(looping PRIVATE -fopt-info-loop -fno-omit-frame-pointer)
target_compile_options(matmult PRIVATE -fopt-info-loop -fno-omit-frame-pointer)

link_directories(googlebenchmark)
target_link_libraries(looping benchmark)
target_link_libraries(matmult benchmark)

add_custom_target(perftest DEPENDS benchmark looping matmult)
add_custom_command(TARGET perftest POST_BUILD COMMAND looping COMMAND matmult)