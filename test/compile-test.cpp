#include "AVArray/Tensor.hpp"
#include <iostream>
#include <cstdlib>

int main(int argc, char const *argv[])
{
	int N = 2; //std::atoi(argv[1]);
	// constexpr static std::align_val_t ALIGN = static_cast<std::align_val_t>(8*sizeof(double));

    Tensor<double, 3> mat_a(N, N, N), mat_b(N, N, N);
    Tensor<double, 4> mat_c(N, N, N, N);

	for (int i = 0; i < N; ++i)
	{
		for (int j = 0; j < N; ++j)
		{
			for (int k = 0; k < N; ++k)
			{
				mat_a[i][j][k] = i*j;
				mat_b(i, j, k) = k;
				for (int l = 0; l < N; ++l)
				{
					mat_c[i][j][k][l] = 0.0;
				}
			}
		}
	}

	// mat_c = mat_a*mat_b;

	for (int i = 0; i < N; ++i)
	{
		for (int j = 0; j < N; ++j)
		{
			for (int m = 0; m < N; ++m)
			{
				for (int k = 0; k < N; ++k)
				{
					for (int l = 0; l < N; ++l)
					{
						mat_c[i][j][k][l] += mat_a[i][j][m]*mat_b[m][k][l];
					}
				}
			}
		}
	}

	for (int i = 0; i < N; ++i)
	{
		for (int j = 0; j < N; ++j)
		{
			for (int k = 0; k < N; ++k)
			{
				for (int l = 0; l < N; ++l)
				{
					std::cout << "C(" << i << ", " << j << ", " << k << ", "
						<< l << "): " << mat_c[i][j][k][l] << std::endl;
				}
			}
		}
	}

	return 0;
}
