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
void find_pr(char *res, char *primes, int start_n, int end_n)
{
	int j, k, offset;
	if(end_n < start_n)
		return;
	for(k = start_n; k <= end_n; k++)
		primes[to_i(k, start_n)] = 1; //обнуляем массив простых чисел
	if(start_n == 1)
		primes[0] = 0; //1-не простое
	offset = start_n % 2;
	int start = start_n + offset;
	if(start == 2)
		start = 4;
	for(j = start; j <= end_n; j += 2)
		primes[to_i(j, start_n)] = 0; //исключаем четные
	for(k = 3; k*k <= end_n; k++) //идем по массиву простых
		if(res[to_i(k, 1)]) //если число простое
		{
			offset = (k-start_n % k)%k;
			start = start_n + offset;
			if(start == k)
				start = k*k;
			for(j = start; j <= end_n; j += k)
				primes[to_i(j, start_n)] = 0;
		}

}
void sieve_non_rec(char *res, int n)
{
	int j, k;
	for(k = 1; k <= n; k++)
		res[to_i(k, 1)] = 1;
	res[to_i(1, 1)] = 0; // 1 - не простое число
	for(k = 1; k*k <= n; k++)
		if(res[to_i(k, 1)])
		{
			if(k == 2)
				for(j = 4; j <= n; j+=2)
					res[to_i(j, 1)] = 0;
			else
				for(j = k*k; j <= n; j+=2*k)
					res[to_i(j, 1)] = 0;
		}
}
int main()
{
	int n = 100;
	char *res = (char*) malloc(n*sizeof(char));
	int i, j, k;
	sieve_non_rec(res, n);
	printf("\n");
	for(k = 1; k <= n; k++)
		if(res[to_i(k, 1)])
			printf("%d, ", k);
	printf("\n");
	int start_n = 1;
	int end_n = 100;
	char *primes = (char*) malloc((end_n-start_n+1)*sizeof(char));
	find_pr(res, primes, start_n, end_n);
	for(k = start_n; k <= end_n; k++)
		if(primes[to_i(k, start_n)])
		{
			if(!prime(k))
				printf("!");
			printf("%d, ", k);
		}
	printf("\n");
}