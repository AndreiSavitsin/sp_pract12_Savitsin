#include <iostream>

int main(int argc, char* argv[])
{
	setlocale(LC_ALL, "Russian");

	if (argc < 3)
	{
		std::cout << "Ошибка: не переданы аргументы\n";
		return 0;
	}	

	int start = atoi(argv[1]);
	int end = atoi(argv[2]);

	int maxDivs = 0;
	int bestNum = start;

	std::cout << "Поиск числа с максимальным количеством делителей:\n";
	for (int i = start; i <= end; i++)
	{
		int divs = 0;
		for (int j = 1; j <= i; j++)
		{
			if (i % j == 0)
			{
				divs++;
			}
		}

		std::cout << "Число: " << i << ". Делителей: " << divs << std::endl;

		if (divs > maxDivs)
		{
			maxDivs = divs;
			bestNum = i;
		}
	}

	std::cout << "Число с наибольшим количеством делителей: " << bestNum << ". Делителей: " << maxDivs << std::endl;

	system("pause");

	return 0;
}