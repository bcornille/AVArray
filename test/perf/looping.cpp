#include <benchmark/benchmark.h>
#include <AVArray/AVArray.hpp>

static void pointer(benchmark::State& state)
{
	for(auto _ : state) {
		double* array_2d = new double[state.range(0)*state.range(0)];

		for (unsigned int i = 0; i < state.range(0); ++i)
		{
			for (unsigned int j = 0; j < state.range(0); ++j)
			{
				array_2d[i*state.range(0)+j] = i*j;
			}
		}
		delete[] array_2d;
	}
}
BENCHMARK(pointer)->Range(8, 1<<10);

static void bracket_operator(benchmark::State& state)
{
	for(auto _ : state) {
		AVArray<double, 2> array_2d(static_cast<unsigned int>(state.range(0)),
			static_cast<unsigned int>(state.range(0)));

		for (unsigned int i = 0; i < state.range(0); ++i)
		{
			for (unsigned int j = 0; j < state.range(0); ++j)
			{
				array_2d[i][j] = i*j;
			}
		}
	}
}
BENCHMARK(bracket_operator)->Range(8, 1<<10);

static void parentheses_operator(benchmark::State& state)
{
	for(auto _ : state) {
		AVArray<double, 2> array_2d(static_cast<unsigned int>(state.range(0)),
			static_cast<unsigned int>(state.range(0)));

		for (unsigned int i = 0; i < state.range(0); ++i)
		{
			for (unsigned int j = 0; j < state.range(0); ++j)
			{
				array_2d(i, j) = i*j;
			}
		}
	}
}
BENCHMARK(parentheses_operator)->Range(8, 1<<10);

BENCHMARK_MAIN();
