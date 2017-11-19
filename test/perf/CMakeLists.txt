add_subdirectory(googlebenchmark)

include_directories(${PROJECT_SOURCE_DIR}/include googlebenchmark/include)

add_executable(looping looping.cpp ${PROJECT_SOURCE_DIR}/include/AVArray/AVArray.hpp googlebenchmark/include/benchmark/benchmark.h)
add_executable(matmult matmult.cpp ${PROJECT_SOURCE_DIR}/include/AVArray/AVArray.hpp googlebenchmark/include/benchmark/benchmark.h)

if("${CMAKE_CXX_COMPILER_ID}" MATCHES "GNU")
	target_compile_options(looping PRIVATE -fopt-info-vec -fno-omit-frame-pointer -march=native)
	target_compile_options(matmult PRIVATE -fopt-info-vec -fno-omit-frame-pointer -march=native)
elseif("${CMAKE_CXX_COMPILER_ID}" MATCHES "Cray")
	target_compile_options(looping PRIVATE -h report=v -fno-omit-frame-pointer -march=native)
	target_compile_options(matmult PRIVATE -h report=v -fno-omit-frame-pointer -march=native)
elseif("${CMAKE_CXX_COMPILER_ID}" MATCHES "Intel")
	target_compile_options(looping PRIVATE -qopt-report=1 -qopt-report-file=stdout -qopt-report-phase=vec -fno-omit-frame-pointer -march=native)
	target_compile_options(matmult PRIVATE -qopt-report=1 -qopt-report-file=stdout -qopt-report-phase=vec -fno-omit-frame-pointer -march=native)
elseif("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
	target_compile_options(looping PRIVATE -Rpass=loop-vectorize -fno-omit-frame-pointer -march=native)
	target_compile_options(matmult PRIVATE -Rpass=loop-vectorize -fno-omit-frame-pointer -march=native)
endif()

link_directories(googlebenchmark)
target_link_libraries(looping benchmark)
target_link_libraries(matmult benchmark)

add_custom_target(perftest DEPENDS benchmark looping matmult)
add_custom_command(TARGET perftest POST_BUILD COMMAND looping COMMAND matmult)
