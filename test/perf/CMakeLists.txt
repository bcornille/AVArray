add_subdirectory(googlebenchmark)

include_directories(${PROJECT_SOURCE_DIR}/include googlebenchmark/include)

add_executable(looping looping.cpp ${PROJECT_SOURCE_DIR}/include/AVArray/AVArray.hpp googlebenchmark/include/benchmark/benchmark.h)
add_executable(sum sum.cpp ${PROJECT_SOURCE_DIR}/include/AVArray/Tensor.hpp googlebenchmark/include/benchmark/benchmark.h)
add_executable(matmult matmult.cpp ${PROJECT_SOURCE_DIR}/include/AVArray/AVArray.hpp googlebenchmark/include/benchmark/benchmark.h)

foreach(target looping matmult sum)
	if("${CMAKE_CXX_COMPILER_ID}" MATCHES "GNU")
		target_compile_options(${target} PRIVATE -fopt-info-vec -fno-omit-frame-pointer -march=native)
	elseif("${CMAKE_CXX_COMPILER_ID}" MATCHES "Cray")
		target_compile_options(${target} PRIVATE -h report=v -fno-omit-frame-pointer -march=native)
	elseif("${CMAKE_CXX_COMPILER_ID}" MATCHES "Intel")
		target_compile_options(${target} PRIVATE -qopt-report=1 -qopt-report-file=stdout -qopt-report-phase=vec -fno-omit-frame-pointer -march=native)
	elseif("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
		target_compile_options(${target} PRIVATE -Rpass=loop-vectorize -fno-omit-frame-pointer -march=native)
	endif()
endforeach(target)

link_directories(googlebenchmark)
target_link_libraries(looping benchmark)
target_link_libraries(sum benchmark)
target_link_libraries(matmult benchmark)

add_custom_target(perftest DEPENDS benchmark looping matmult sum)
add_custom_command(TARGET perftest POST_BUILD COMMAND looping COMMAND sum COMMAND matmult)
