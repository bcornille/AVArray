#include "AVArray/AVArray.hpp"
#include <iostream>

int main(int argc, char const *argv[])
{
	AVArray<double, 2> mat_a(1024u, 1024u), mat_b(1024u, 1024u), mat_c(1024u, 1024u);

	for (unsigned int i = 0; i < 1024; ++i)
	{
		for (unsigned int j = 0; j < 1024; ++j)
		{
			mat_a(i, j) = i;
			mat_b(i, j) = j;
			mat_c(i, j) = 0.0;
		}
	}

	for (unsigned int i = 0; i < 1024; ++i)
	{
		for (unsigned int k = 0; k < 1024; ++k)
		{
			for (unsigned int j = 0; j < 1024; ++j)
			{
				mat_c(i, j) += mat_a(i, k)*mat_b(k, j);
			}
		}
	}

	return 0;
}
