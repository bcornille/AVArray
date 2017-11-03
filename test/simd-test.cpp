#include "AVArray/MyVector.hpp"
#include <iostream>
#include <vector>

int main(int argc, char const *argv[])
{
	MyVector<int> test_my_vector(10);
	std::vector<int> test_std_vector(10);

	for (int i = 0; i < test_my_vector.size(); ++i)
	{
		test_my_vector(i) = i;
	}

	std::cout << test_my_vector[test_my_vector.size() - 1] << std::endl;

	for (int i = 0; i < test_std_vector.size(); ++i)
	{
		test_std_vector[i] = i;
	}

	std::cout << test_std_vector[test_std_vector.size() - 1] << std::endl;

	return 0;
}