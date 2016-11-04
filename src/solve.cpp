#include <stdio.h>
#include <math.h>
#include "mpi.h"

int rank, proc_num;

void sieve(int n, char *res)
{
	int i, j, k, tmp;
	if(n <= 100)
	{
		for(i = 0; i < n; i++)
			res[i] = 1;
		res[0] = 0; // 1 - не простое число
		for(k = 1; k*k <= n; k++)
			if(res[k-1])
				if(k == 2)
					for(j = 4; j <= n; j+=2)
						res[j-1] = 0;
				else
					for(j = k*k; j <= n; j+=2*k)
						res[j-1] = 0;
	}
	else
	{
		int sqr_n = int(floor(sqrt(n))); //количество чисел до корня из n
		sieve(sqr_n, res); //получаем первые sqr_n простых чисел
		int part_size = int(floor((float(n) - sqr_n)/proc_num)); //делим оставшуюся область, каждому поровну
		int ar[2]; ar[0] = sqr_n; ar[1] = part_size; //передаем начальное количество простых чисел и размер части
		MPI_Bcast(ar, 2, MPI_INT, 0, MPI_COMM_WORLD); //всем рабам и мастеру выделить буфер для приема простых чисел
		MPI_Bcast(res, sqr_n, MPI_CHAR, 0, MPI_COMM_WORLD); //собственно раздача простых чисел

		{
			int start_n = ar[0] + rank*ar[1] + 1;
			int end_n = start_n + ar[1] - 1;
			for(k = 1; k*k <= end_n; k++) //TODO: обнулить перед изменением
				if(res[k-1])
					if(k == 2)
						for(j = start_n + 2 - (start_n % 2); j <= end_n; j += 2)
							res[j-1] = 0;
					else
						for(j = start_n + k - (start_n % k); j <= end_n; j += 2*k)
							res[j-1] = 0;
			MPI_Gather(res + start_n - 1, ar[1], MPI_CHAR, res + ar[0], ar[1], MPI_CHAR, 0, MPI_COMM_WORLD);
		}
		if((tmp = n%proc_num) != 0)
		{

			for(i = 0; i < tmp; i++) //res[n+i] = check(n+i+1);
			{
				for(k = 1; k*k <= n; k++)
					if(res[k-1])
						if(k == 2)
							for(j = )
			}
				
		}
		MPI_
		for(i = 1; i < proc_num; i++)
			MPI_Isend(pr, n_pr, MPI_CHAR, i, n, MPI_COMM_WORLD, &request)
		
		char *recv_res = (char*) malloc(part_size + proc_num);

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
    	
    	sieve(n, res);
    	int i;
    	for(i = 0; i < n; i++)
			if(res[i])
				printf("%d, ", i+1);
		printf("\n");
    }
    else
    {
    	int n, size;
    	while(1)
    	{
    		MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);

    	}
    }
    
    MPI_Finalize();
    
    return 0;
}
