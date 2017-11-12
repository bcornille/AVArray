#include <benchmark/benchmark.h>
#include <AVArray/AVArray.hpp>

static void pointer(benchmark::State& state)
{
	for(auto _ : state) {
		unsigned int N = state.range(0);
		double* array_2d = new double[N*N];

		for (unsigned int i = 0; i < N; ++i)
		{
			for (unsigned int j = 0; j < N; ++j)
			{
				array_2d[i*N+j] = i*j;
			}
		}
		benchmark::DoNotOptimize(array_2d);
		delete[] array_2d;
	}
}
BENCHMARK(pointer)->Range(8, 1<<10);

static void bracket_operator(benchmark::State& state)
{
	for(auto _ : state) {
		unsigned int N = state.range(0);
		AVArray<double, 2> array_2d(N, N);

		for (unsigned int i = 0; i < N; ++i)
		{
			for (unsigned int j = 0; j < N; ++j)
			{
				array_2d[i][j] = i*j;
			}
		}
		benchmark::DoNotOptimize(&array_2d[0u][0u]);
	}
}
BENCHMARK(bracket_operator)->Range(8, 1<<10);

static void parentheses_operator(benchmark::State& state)
{
	for(auto _ : state) {
		unsigned int N = state.range(0);
		AVArray<double, 2> array_2d(N, N);

		for (unsigned int i = 0; i < N; ++i)
		{
			for (unsigned int j = 0; j < N; ++j)
			{
				array_2d(i, j) = i*j;
			}
		}
		benchmark::DoNotOptimize(&array_2d(0u, 0u));
	}
}
BENCHMARK(parentheses_operator)->Range(8, 1<<10);

BENCHMARK_MAIN();
