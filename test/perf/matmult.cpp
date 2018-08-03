#include <benchmark/benchmark.h>
#include <AVArray/AVArray.hpp>

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
				mat_c[i*N+j] = 0.0;
			}
		}

		for (int i = 0; i < N; ++i)
		{
			for (int k = 0; k < N; ++k)
			{
				for (int j = 0; j < N; ++j)
				{
					mat_c[i*N+j] += mat_a[i*N+k]*mat_b[k*N+j];
				}
			}
		}

		delete[] mat_a;
		delete[] mat_b;
		delete[] mat_c;
	}
}
BENCHMARK(pointer)->Range(8, 1<<8);

static void double_pointer(benchmark::State& state)
{
	for(auto _ : state) {
		int N = state.range(0);
		double** mat_a = new double*[N];
		double** mat_b = new double*[N];
		double** mat_c = new double*[N];

		for (int i = 0; i < N; ++i)
		{
			mat_a[i] = new double[N];
			mat_b[i] = new double[N];
			mat_c[i] = new double[N];
		}

		for (int i = 0; i < N; ++i)
		{
			for (int j = 0; j < N; ++j)
			{
				mat_a[i][j] = i;
				mat_b[i][j] = j;
				mat_c[i][j] = 0.0;
			}
		}

		for (int i = 0; i < N; ++i)
		{
			for (int k = 0; k < N; ++k)
			{
				for (int j = 0; j < N; ++j)
				{
					mat_c[i][j] += mat_a[i][k]*mat_b[k][j];
				}
			}
		}

		for (int i = 0; i < N; ++i)
		{
			delete[] mat_a[i];
			delete[] mat_b[i];
			delete[] mat_c[i];
		}

		delete[] mat_a;
		delete[] mat_b;
		delete[] mat_c;
	}
}
BENCHMARK(double_pointer)->Range(8, 1<<8);

static void bracket_operator(benchmark::State& state)
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
				mat_c[i][j] = 0.0;
			}
		}

		for (int i = 0; i < N; ++i)
		{
			for (int k = 0; k < N; ++k)
			{
				for (int j = 0; j < N; ++j)
				{
					mat_c[i][j] += mat_a[i][k]*mat_b[k][j];
				}
			}
		}
	}
}
BENCHMARK(bracket_operator)->Range(8, 1<<8);

static void parentheses_operator(benchmark::State& state)
{
	for(auto _ : state) {
		int N = state.range(0);
		AVArray<double, 2> mat_a(N, N), mat_b(N, N), mat_c(N, N);

		for (int i = 0; i < N; ++i)
		{
			for (int j = 0; j < N; ++j)
			{
				mat_a(i, j) = i;
				mat_a(i, j) = j;
				mat_a(i, j) = 0.0;
			}
		}

		for (int i = 0; i < N; ++i)
		{
			for (int k = 0; k < N; ++k)
			{
				for (int j = 0; j < N; ++j)
				{
					mat_c(i, j) += mat_a(i, k)*mat_b(k, j);
				}
			}
		}
	}
}
BENCHMARK(parentheses_operator)->Range(8, 1<<8);

BENCHMARK_MAIN();
