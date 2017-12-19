add_subdirectory(googlebenchmark)

include_directories(${PROJECT_SOURCE_DIR}/include googlebenchmark/include ~/usr/include)

foreach(target looping matmult sum)
	add_executable(${target} ${target}.cpp ${PROJECT_SOURCE_DIR}/include/AVArray/AVArray.hpp googlebenchmark/include/benchmark/benchmark.h)
	if("${CMAKE_CXX_COMPILER_ID}" MATCHES "GNU")
		target_compile_options(${target} PRIVATE -fopt-info-vec -fno-omit-frame-pointer -march=native)
	elseif("${CMAKE_CXX_COMPILER_ID}" MATCHES "Cray")
		target_compile_options(${target} PRIVATE -h report=v -march=native)
	elseif("${CMAKE_CXX_COMPILER_ID}" MATCHES "Intel")
		target_compile_options(${target} PRIVATE -qopt-report=5 -qopt-report-file=stderr -qopt-report-phase=vec -march=native)
	elseif("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
		target_compile_options(${target} PRIVATE -Rpass=loop-vectorize -fno-omit-frame-pointer -march=native)
	endif()
endforeach(target)

link_directories(googlebenchmark)
target_link_libraries(looping benchmark)
target_link_libraries(sum benchmark)
target_link_libraries(matmult benchmark)

add_custom_target(perftest DEPENDS benchmark looping sum matmult)
add_custom_command(TARGET perftest POST_BUILD COMMAND looping COMMAND sum COMMAND matmult)
