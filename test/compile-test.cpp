#include "AVArray/Tensor.hpp"
#include <iostream>
#include <cstdlib>

int main(int argc, char const *argv[])
{
	int N = 2; //std::atoi(argv[1]);
	// constexpr static std::align_val_t ALIGN = static_cast<std::align_val_t>(8*sizeof(double));

    Tensor<double, 3> mat_a(N, N, N), mat_b(N, N, N);
    Tensor<double, 3> mat_c(N, N, N), mat_d(N, N, N);

	for (int i = 0; i < N; ++i)
	{
		for (int j = 0; j < N; ++j)
		{
			for (int k = 0; k < N; ++k)
			{
				mat_a[i][j][k] = i*j;
				mat_b(i, j, k) = k;
				mat_c[i][j][k] = 0.0;
				mat_d[i][j][k] = 0.0;
			}
		}
	}

	mat_c = mat_a + mat_b;

	for (int i = 0; i < N; ++i)
	{
		for (int j = 0; j < N; ++j)
		{
			for (int k = 0; k < N; ++k)
			{
				mat_d[i][j][k] = mat_a[i][j][k] + mat_b[i][j][k];
			}
		}
	}

	for (int i = 0; i < N; ++i)
	{
		for (int j = 0; j < N; ++j)
		{
			for (int k = 0; k < N; ++k)
			{
				std::cout << "C(" << i << ", " << j << ", " << k << ", "
					<< "): " << mat_c[i][j][k] << std::endl;
				std::cout << "D(" << i << ", " << j << ", " << k << ", "
					<< "): " << mat_d[i][j][k] << std::endl;
			}
		}
	}

	return 0;
}
