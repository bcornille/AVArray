#include <benchmark/benchmark.h>
#include <AVArray/Tensor.hpp>

static void pointer(benchmark::State& state)
{
	for(auto _ : state) {
		int N = state.range(0);
		double* mat_a = new double[N*N];
		double* mat_b = new double[N*N];
		double* mat_c = new double[N*N];

		for (int i = 0; i < N; ++i)
		{
			for (int j = 0; j < N; ++j)
			{
				mat_a[i*N+j] = i;
				mat_b[i*N+j] = j;
			}
		}

		for (int i = 0; i < N; ++i)
		{
			for (int j = 0; j < N; ++j)
			{
				mat_c[i*N+j] = mat_a[i*N+j] + mat_b[i*N+j];
			}
		}

		delete[] mat_a;
		delete[] mat_b;
		delete[] mat_c;
	}
}
BENCHMARK(pointer)->Range(8, 1<<10);

static void array_bracket(benchmark::State& state)
{
	for(auto _ : state) {
		int N = state.range(0);
		AVArray<double, 2> mat_a(N, N), mat_b(N, N), mat_c(N, N);

		for (int i = 0; i < N; ++i)
		{
			for (int j = 0; j < N; ++j)
			{
				mat_a[i][j] = i;
				mat_b[i][j] = j;
			}
		}

		for (int i = 0; i < N; ++i)
		{
			for (int j = 0; j < N; ++j)
			{
				mat_c[i][j] = mat_a[i][j] + mat_b[i][j];
			}
		}
	}
}
BENCHMARK(array_bracket)->Range(8, 1<<10);

static void tensor_bracket(benchmark::State& state)
{
	for(auto _ : state) {
		int N = state.range(0);
		Tensor<double, 2> mat_a(N, N), mat_b(N, N), mat_c(N, N);

		for (int i = 0; i < N; ++i)
		{
			for (int j = 0; j < N; ++j)
			{
				mat_a[i][j] = i;
				mat_b[i][j] = j;
			}
		}

		for (int i = 0; i < N; ++i)
		{
			for (int j = 0; j < N; ++j)
			{
				mat_c[i][j] = mat_a[i][j] + mat_b[i][j];
			}
		}
	}
}
BENCHMARK(tensor_bracket)->Range(8, 1<<10);

static void tensor(benchmark::State& state)
{
	for(auto _ : state) {
		int N = state.range(0);
		Tensor<double, 2> mat_a(N, N), mat_b(N, N), mat_c(N, N);

		for (int i = 0; i < N; ++i)
		{
			for (int j = 0; j < N; ++j)
			{
				mat_a[i][j] = i;
				mat_b[i][j] = j;
			}
		}

		mat_c = 2.0*mat_a + mat_b;
	}
}
BENCHMARK(tensor)->Range(8, 1<<10);

static void tensor_bracket_3(benchmark::State& state)
{
	for(auto _ : state) {
		int N = state.range(0);
		Tensor<double, 2> mat_a(N, N), mat_b(N, N), mat_c(N, N), mat_d(N, N);

		for (int i = 0; i < N; ++i)
		{
			for (int j = 0; j < N; ++j)
			{
				mat_a[i][j] = i;
				mat_b[i][j] = j;
				mat_c[i][j] = i*j;
			}
		}

		for (int i = 0; i < N; ++i)
		{
			for (int j = 0; j < N; ++j)
			{
				mat_d[i][j] = mat_a[i][j] + mat_b[i][j] + mat_c[i][j];
			}
		}
	}
}
BENCHMARK(tensor_bracket_3)->Range(8, 1<<10);

static void tensor_3(benchmark::State& state)
{
	for(auto _ : state) {
		int N = state.range(0);
		Tensor<double, 2> mat_a(N, N), mat_b(N, N), mat_c(N, N), mat_d(N, N);

		for (int i = 0; i < N; ++i)
		{
			for (int j = 0; j < N; ++j)
			{
				mat_a[i][j] = i;
				mat_b[i][j] = j;
				mat_c[i][j] = i*j;
			}
		}

		mat_d = mat_a + mat_b + mat_c;
	}
}
BENCHMARK(tensor_3)->Range(8, 1<<10);

BENCHMARK_MAIN();
