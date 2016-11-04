#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "mpi.h"

#define debug 0

int rank, proc_num;

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
	for(k = start_n; k <= end_n; k++)
		primes[to_i(k, start_n)] = 1; //обнуляем массив простых чисел
	offset = start_n % 2;
	for(j = start_n + (offset == 0? 0 : 2-offset); j <= end_n; j += 2)
		primes[to_i(j, start_n)] = 0; //исключаем четные
	for(k = 3; k*k <= end_n; k++) //идем по массиву простых
		if(res[to_i(k, 1)]) //если число простое
		{
			offset = start_n % k;
			for(j = start_n + (offset == 0? 0 : k-offset); j <= end_n; j += k)
				primes[to_i(j, start_n)] = 0;
		}
	if(debug) fprintf(stderr, "Found primes at [%d, %d]\n", start_n, end_n);
}
void sieve_non_rec(char *res, int n)
{
	if(debug) fprintf(stderr, "Entered non-recursive part with n=%d, res=%d\n", n, int(res));
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
// void sieve_first_n(int n, char *res)
//  {
//  	int k, offset;
//  	if(n <= 100)
//  		sieve_non_rec(res, n);
//  	else
//  	{
//  		int sqr_n = int(floor(sqrt(n))); //количество чисел до корня из n
//  		sieve(sqr_n, res); //получаем первые sqr_n простых чисел
//  		int part_size = int(floor((float(n) - sqr_n)/proc_num)); //делим оставшуюся область, каждому поровну
//  		int ar[2]; ar[0] = sqr_n; ar[1] = part_size; //передаем начальное количество простых чисел и размер части
//  		MPI_Bcast(ar, 2, MPI_INT, 0, MPI_COMM_WORLD); //всем рабам и мастеру выделить буфер для приема простых чисел
//  		MPI_Bcast(res, sqr_n, MPI_CHAR, 0, MPI_COMM_WORLD); //собственно раздача простых чисел
 
//  		// каждый процесс вычисляет свою область
//  		{
//  			// res = malloc(ar[1])
//  			int start_n = ar[0] + rank*ar[1] + 1;
//  			int end_n = start_n + ar[1] - 1;
//  			char *primes = (char*) malloc(end_n - start_n + 1);
//  			find_pr(res, primes, start_n, end_n);
//  			MPI_Gather(primes, ar[1], MPI_CHAR, res + ar[0], ar[1], MPI_CHAR, 0, MPI_COMM_WORLD);
//  			free(primes);
//  			// free(res);
//  		}
//  		if((offset = (n-sqr_n)%proc_num) != 0)
//  		{
//  			if(debug) fprintf(stderr, "Не делится %d на %d: остаток %d\n", n, proc_num, offset);
//  			int start_n = n-offset+1;
//  			int end_n = n;
//  			char *primes = (char*) malloc(end_n - start_n + 1);
//  			find_pr(res, primes, start_n, end_n);
//  			for(k = start_n; k <= end_n; k++)
//  				res[to_i(k, 1)] = primes[to_i(k, start_n)];
//  			free(primes);
//  		}
//  	}
//  }
void sieve(char *res, char *primes, int start, int end)
{
	int offset;
	int sqr_n = int(floor(sqrt(end))); // количество чисел до корня из n
	if(sqr_n <= 100)
		sieve_non_rec(res, sqr_n);
	else
	{
		char *res_tmp = (char*) malloc(int(floor(sqrt(sqr_n)))*sizeof(char));
		sieve(res_tmp, res, 1, sqr_n); // получаем первые sqr_n простых чисел
		free(res_tmp);
	}
		
	// так или иначе, мы получаем в res первые sqr_n простых чисел
	int part_size = int(floor((float(end) - start + 1)/proc_num)); //чисел, включая концы, +1
	int ar[3]; ar[0] = sqr_n; ar[1] = part_size; ar[2] = start; //передаем начальное количество простых чисел, размер части и стартовую точку
	MPI_Bcast(ar, 3, MPI_INT, 0, MPI_COMM_WORLD); //всем рабам и мастеру выделить буфер для приема простых чисел
	MPI_Bcast(res, sqr_n, MPI_CHAR, 0, MPI_COMM_WORLD); //собственно раздача простых чисел

	// каждый процесс вычисляет свою область
	{
		if(debug) fprintf(stderr, "Процесс %d получил приказ: part_size = %d, начало в %d\n", rank, ar[1], ar[2]);
		int start_n = ar[2] + rank*ar[1];
		int end_n = start_n + ar[1] - 1;
		char *primes_tmp = (char*) malloc(ar[1]);
		find_pr(res, primes_tmp, start_n, end_n);
		MPI_Gather(primes_tmp, ar[1], MPI_CHAR, primes, ar[1], MPI_CHAR, 0, MPI_COMM_WORLD);
		free(primes_tmp);
	}
	if((offset = (end-start+1)%proc_num) != 0)
	{
		if(debug) fprintf(stderr, "Не делится область [%d, %d] на %d: остаток %d\n", start, end, proc_num, offset);
		int start_n = end-offset+1;
		int end_n = end;
		find_pr(res, primes+to_i(start_n, start), start_n, end_n);
	}
}

int main (int argc, char* argv[])
{
    
    MPI_Init (&argc, &argv);
    MPI_Comm_rank (MPI_COMM_WORLD, &rank);
    MPI_Comm_size (MPI_COMM_WORLD, &proc_num);

    if(rank == 0)
    {
    	// int n = int(1e8);
    	// char *res = (char*) malloc(n*sizeof(char));
    	
  //   	sieve(n, res);
		// int k;
  //   	for(k = 1; k <= n; k++)
		// 	if(res[to_i(k, 1)])
		// 	{
		// 		printf("%d", k);
		// 		if(debug)
		// 			if(!prime(k))
		// 			{
		// 				printf("!!!");
		// 				if(debug) fprintf(stderr, "ERROR! k = %d\n", k);
		// 			}
					
		// 		printf("\n");
		// 	}

    	int start = 100, end = 10000;
    	int k;
    	int sqr_n = int(floor(sqrt(end)));
    	char *res = (char*) malloc(sqr_n*sizeof(char));
    	char *primes = (char*) malloc((end-start+1)*sizeof(char));
    	sieve(res, primes, start, end);
    	for(k = start; k <= end; k++)
    		if(primes[to_i(k, start)])
    		{
    			printf("%d", k);
    			if(debug)
	    			if(!prime(k))
			 		{
						printf("!!!");
						if(debug) fprintf(stderr, "ERROR! k = %d\n", k);
					}
		 		printf("\n");
    		}

    	int ar[3]; ar[1] = 0;
    	MPI_Bcast(ar, 3, MPI_INT, 0, MPI_COMM_WORLD);
    	
				
		free(res);
		MPI_Finalize();
		return 0;
    }
    else
    {
    	while(1)
    	{
    		int ar[3];
    		MPI_Bcast(ar, 3, MPI_INT, 0, MPI_COMM_WORLD);
    		if(ar[1] != 0)
    		{
				char *res = (char*) malloc(ar[0] * sizeof(char));
				MPI_Bcast(res, ar[0], MPI_CHAR, 0, MPI_COMM_WORLD);
				if(debug) fprintf(stderr, "Процесс %d получил приказ: part_size = %d, начало в %d\n", rank, ar[1], ar[2]);
				int start_n = ar[2] + rank*ar[1];
				int end_n = start_n + ar[1] - 1;
				char *primes = (char*) malloc(ar[1]);
				find_pr(res, primes, start_n, end_n);
				MPI_Gather(primes, ar[1], MPI_CHAR, primes, ar[1], MPI_CHAR, 0, MPI_COMM_WORLD);
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
