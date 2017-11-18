#include "AVArray/AVArray.hpp"
#include <iostream>
#include <cstdlib>

int main(int argc, char const *argv[])
{
	unsigned int N = 32; //std::atoi(argv[1]);
	constexpr static std::align_val_t ALIGN = static_cast<std::align_val_t>(8*sizeof(double));

    double* mat_a = new(ALIGN) double[N*N];
	double* mat_b = new(ALIGN) double[N*N];
	double* mat_c = new(ALIGN) double[N*N];

	for (unsigned int i = 0; i < N; ++i)
	{
		for (unsigned int j = 0; j < N; ++j)
		{
			mat_a[i*N+j] = i;
			mat_b[i*N+j] = j;
			mat_c[i*N+j] = 0.0;
		}
	}

	for (unsigned int i = 0; i < N; ++i)
	{
		for (unsigned int k = 0; k < N; ++k)
		{
			for (unsigned int j = 0; j < N; ++j)
			{
				mat_c[i*N+j] += mat_a[i*N+k]*mat_b[k*N+j];
			}
		}
	}

	delete[] mat_a;
	delete[] mat_b;
	delete[] mat_c;

	return 0;
}
