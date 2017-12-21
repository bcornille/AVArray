#include <benchmark/benchmark.h>
#include <AVArray/AVArray.hpp>

static void pointer(benchmark::State& state)
{
	for(auto _ : state) {
		int N = state.range(0);
		double* array_2d = new double[N*N];

		for (int i = 0; i < N; ++i)
		{
			for (int j = 0; j < N; ++j)
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
		int N = state.range(0);
		AVArray<double, 2> array_2d(N, N);

		for (int i = 0; i < N; ++i)
		{
			for (int j = 0; j < N; ++j)
			{
				array_2d[i][j] = i*j;
			}
		}
		benchmark::DoNotOptimize(&array_2d[0][0]);
	}
}
BENCHMARK(bracket_operator)->Range(8, 1<<10);

static void parentheses_operator(benchmark::State& state)
{
	for(auto _ : state) {
		int N = state.range(0);
		AVArray<double, 2> array_2d(N, N);

		for (int i = 0; i < N; ++i)
		{
			for (int j = 0; j < N; ++j)
			{
				array_2d(i, j) = i*j;
			}
		}
		benchmark::DoNotOptimize(&array_2d(0, 0));
	}
}
BENCHMARK(parentheses_operator)->Range(8, 1<<10);

static void bracket_8(benchmark::State& state)
{
	for(auto _ : state) {
		int N = state.range(0);
		AVArray<double, 8> array_8d(N, N, N, N, N, N, N, N);

		for (int i = 0; i < N; ++i)
		{
		for (int j = 0; j < N; ++j)
		{
		for (int k = 0; k < N; ++k)
		{
		for (int l = 0; l < N; ++l)
		{
		for (int m = 0; m < N; ++m)
		{
		for (int n = 0; n < N; ++n)
		{
		for (int o = 0; o < N; ++o)
		{
		for (int p = 0; p < N; ++p)
		{
			array_8d[i][j][k][l][m][n][o][p] = i+j+k+l+m+n+o+p;
		}
		}
		}
		}
		}
		}
		}
		}
		benchmark::DoNotOptimize(&array_8d.data(0));
	}
}
BENCHMARK(bracket_8)->Range(2, 8);

static void parentheses_8(benchmark::State& state)
{
	for(auto _ : state) {
		int N = state.range(0);
		AVArray<double, 8> array_8d(N, N, N, N, N, N, N, N);

		for (int i = 0; i < N; ++i)
		{
		for (int j = 0; j < N; ++j)
		{
		for (int k = 0; k < N; ++k)
		{
		for (int l = 0; l < N; ++l)
		{
		for (int m = 0; m < N; ++m)
		{
		for (int n = 0; n < N; ++n)
		{
		for (int o = 0; o < N; ++o)
		{
		for (int p = 0; p < N; ++p)
		{
			array_8d(i, j, k, l, m, n, o, p) = i+j+k+l+m+n+o+p;
		}
		}
		}
		}
		}
		}
		}
		}
		benchmark::DoNotOptimize(&array_8d.data(0));
	}
}
BENCHMARK(parentheses_8)->Range(2, 8);

BENCHMARK_MAIN();
