#include "AVArray/Tensor.hpp"
#include <iostream>
// #include <cstdlib>

// template<typename T, int D>
// std::ostream& operator<<(std::ostream &os, const Tensor<T, D> &tensor)
// {
// 	for (int i = 0; i < tensor.leadingDim(); ++i)
// 	{
// 		os << tensor[i];
// 	}
// 	os << std::endl;
// 	return os;
// }

int main(int argc, char const *argv[])
{
	int N = 2;
	Tensor<double, 2> mat_a(N, N), mat_b(N, N), mat_c(N, N), mat_d(N, N);

	for (int i = 0; i < N; ++i)
	{
		for (int j = 0; j < N; ++j)
		{
			mat_a[i][j] = i + 1;
			mat_b[i][j] = j + 1;
		}
	}

	mat_c = 1.0 / mat_a * mat_b;

	for (int i = 0; i < N; ++i)
	{
		for (int j = 0; j < N; ++j)
		{
			mat_d[i][j] = 1.0 / mat_a[i][j] * mat_b[i][j];
		}
	}

	for (int i = 0; i < N; ++i)
	{
		for (int j = 0; j < N; ++j)
		{
			// if (mat_c[i][j] != mat_d[i][j])
			// {
				std::cout << "C(" << i << ", " << j << "): " << mat_c[i][j] << std::endl;
				std::cout << "D(" << i << ", " << j << "): " << mat_d[i][j] << std::endl;
			// }
		}
	}


	return 0;
}
