#include "AVArray/Tensor.hpp"
#include <iostream>
#include <cstdlib>

int main(int argc, char const *argv[])
{
	unsigned int N = 4; //std::atoi(argv[1]);
	// constexpr static std::align_val_t ALIGN = static_cast<std::align_val_t>(8*sizeof(double));

    Tensor<double, 2> mat_a(N, N), mat_b(N, N), mat_c(N, N), mat_d(N, N);

	for (unsigned int i = 0; i < N; ++i)
	{
		for (unsigned int j = 0; j < N; ++j)
		{
			mat_a[i][j] = i;
			mat_b[i][j] = j;
			mat_c[i][j] = 0.0;
			mat_d[i][j] = 0.0;
		}
	}

	mat_c = mat_a*mat_b;

	for (unsigned int i = 0; i < N; ++i)
	{
		for (unsigned int k = 0; k < N; ++k)
		{
			for (unsigned int j = 0; j < N; ++j)
			{
				mat_d[i][j] += mat_a[i][k]*mat_b[k][j];
			}
		}
	}

	for (int i = 0; i < N; ++i)
	{
		for (int j = 0; j < N; ++j)
		{
			if (mat_c[i][j] != mat_d[i][j])
			{
				std::cout << "(i, j): (" << i << ", " << j << ")" << std::endl;
				std::cout << "Matrix C: " << mat_c[i][j] << std::endl;
				std::cout << "Matrix D: " << mat_d[i][j] << std::endl;
			}
		}
	}

	return 0;
}
