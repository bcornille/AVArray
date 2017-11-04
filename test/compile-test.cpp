#include "AVArray/AVArray.hpp"

int main(int argc, char const *argv[])
{
	AVArray<double, 2> matrix(1024u, 1024u);

	for (int i = 0; i < 1024; ++i)
	{
		for (int j = 0; j < 1024; ++j)
		{
			matrix[i][j] = i*j;
		}
	}

	return 0;
}