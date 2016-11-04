#include <stdio.h>
#include <stdlib.h>

char prime(int n)
{
	int k;
	for(k = 2; k < n/2; k++)
		if(n%k == 0)
			return 0;
	return 1;
}
int to_i(int n, int start)
{
	return n-start;
}
int at(int i, int start)
{
	return i+start;
}
void foo(char *res, char *primes, int start_n, int end_n)
{
	int i, j, k;
	for(k = start_n; k <= end_n; k++)
		primes[to_i(k, start_n)] = 1; //обнуляем массив простых чисел
	for(j = start_n + 2 - (start_n % 2); j <= end_n; j += 2)
		primes[to_i(j, start_n)] = 0; //исключаем четные
	for(k = 3; k*k <= end_n; k++) //идем по массиву простых
		if(res[to_i(k, 1)]) //если число простое
			for(j = start_n + k - (start_n % k); j <= end_n; j += k)
				primes[to_i(j, start_n)] = 0;
	for(i = 0; i < 100; i++)
		printf("%d, ", res[i]);
	printf("\n");
}

void bar(char *res, int n)
{
	int j, k;
	for(k = 1; k <= n; k++)
		res[to_i(k, 1)] = 1;
	res[to_i(1, 1)] = 0; // 1 - не простое число
	for(k = 1; k*k <= n; k++)
		if(res[to_i(k, 1)])
			if(k == 2)
				for(j = 4; j <= n; j+=2)
					res[to_i(j, 1)] = 0;
			else
				for(j = k*k; j <= n; j+=2*k)
					res[to_i(j, 1)] = 0;
}

int main()
{
	int n = 100;
	char *res = (char*) malloc(n*sizeof(char));
	int i, j, k;
	bar(res, n);
	for(k = 1; k <= n; k++)
		printf("%d, ", res[to_i(k, 1)]);
	printf("\n");
	for(k = 1; k <= n; k++)
		if(res[to_i(k, 1)])
			printf("%d, ", k);
	printf("\n");
	int start_n = n + 1;
	int end_n = n + 1100;
	char *primes = (char*) malloc((end_n-start_n+1)*sizeof(char));
	foo(res, primes, start_n, end_n);
	for(k = start_n; k <= end_n; k++)
		if(primes[to_i(k, start_n)])
		{
			if(!prime(k))
				printf("!");
			printf("%d, ", k);
		}
	printf("\n");
}