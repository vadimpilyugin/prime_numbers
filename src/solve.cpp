#include <stdio.h>
#include <math.h>
#include "mpi.h"

int rank, proc_num;

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
void sieve_non_rec(char *res, int n)
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

void sieve(int n, char *res)
{
	int i, j, k, tmp;
	if(n <= 100)
		sieve_non_rec(res, n);
	else
	{
		int sqr_n = int(floor(sqrt(n))); //количество чисел до корня из n
		sieve(sqr_n, res); //получаем первые sqr_n простых чисел
		int part_size = int(floor((float(n) - sqr_n)/proc_num)); //делим оставшуюся область, каждому поровну
		int ar[2]; ar[0] = sqr_n; ar[1] = part_size; //передаем начальное количество простых чисел и размер части
		MPI_Bcast(ar, 2, MPI_INT, 0, MPI_COMM_WORLD); //всем рабам и мастеру выделить буфер для приема простых чисел
		MPI_Bcast(res, sqr_n, MPI_CHAR, 0, MPI_COMM_WORLD); //собственно раздача простых чисел

		// каждый процесс вычисляет свою область
		{
			// res = malloc(ar[1])
			int start_n = ar[0] + rank*ar[1] + 1;
			int end_n = start_n + ar[1] - 1;
			char *primes = (char*) malloc(end_n - start_n + 1);
			find_pr(res, primes, start_n, end_n);
			MPI_Gather(primes, ar[1], MPI_CHAR, res + ar[0], ar[1], MPI_CHAR, 0, MPI_COMM_WORLD);
			free(primes);
			// free(res);
		}
		if((offset = n%proc_num) != 0)
		{
			int start_n = n-offset+1;
			int end_n = n;
			char *primes = (char*) malloc(end_n - start_n + 1);
			find_pr(res, primes, start_n, end_n);
			for(int k = start_n; k <= end_n; k++)
				res[to_i(k, 1)] = primes[to_i(k, start_n)];
			free(primes);
		}
	}
}

int main (int argc, char* argv[])
{
    
    MPI_Init (&argc, &argv);
    MPI_Comm_rank (MPI_COMM_WORLD, &rank);
    MPI_Comm_size (MPI_COMM_WORLD, &proc_num);

    if(rank == 0)
    {
    	int n = 10000;
    	char *res = (char*) malloc(n*sizeof(char));
    	
    	//sieve(n, res);
    	sieve_non_rec(res, n);

    	int ar[2]; ar[1] = 0;
    	MPI_Bcast(ar, 2, MPI_INT, 0, MPI_COMM_WORLD);
    	int k;
    	for(k = 1; k <= n; k++)
			if(res[to_i(k)])
				printf("%d ", k);
		printf("\n");
		free(res);
		MPI_Finalize();
		return 0;
    }
    else
    {
    	while(1)
    	{
    		int ar[2];
    		MPI_Bcast(ar, 2, MPI_INT, 0, MPI_COMM_WORLD);
    		if(ar[1] != 0)
    		{
				res = (char*) malloc(ar[0] * sizeof(char));
				MPI_Bcast(res, ar[0], MPI_CHAR, 0, MPI_COMM_WORLD);
				int start_n = ar[0] + rank*ar[1] + 1;
				int end_n = start_n + ar[1] - 1;
				char *primes = (char*) malloc(end_n - start_n + 1);
				find_pr(res, primes, start_n, end_n);
				MPI_Gather(primes, ar[1], MPI_CHAR, res + ar[0], ar[1], MPI_CHAR, 0, MPI_COMM_WORLD);
				free(primes);
				free(res);
			}
			else
			{
				MPI_Finalize();
				return 0;
			}
    	}
    }    
}
