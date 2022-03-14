/* banker_lock.c */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

#define N_ACCOUNTS 10
#define N_THREADS  100
#define N_ROUNDS   10000

struct account
{
	long balance;
	/* add a mutex to prevent races on balance */
	pthread_mutex_t mtx;
} accts[N_ACCOUNTS];

int rand_range(int N)
{
	return (int)((double)rand() / ((double)RAND_MAX + 1) * N);
}

void *disburse(void *arg)
{
	size_t i, from, to;
	long payment;

	(void)arg;

	for (i = 0; i < N_ROUNDS; i++)
	{
		from = rand_range(N_ACCOUNTS);
		do {
			to = rand_range(N_ACCOUNTS);
		} while (to == from);

		/* get an exclusive lock on both balances before
		   updating (there's a problem with this, see below) */
		pthread_mutex_lock(&accts[from].mtx);
		pthread_mutex_lock(&accts[to].mtx);
		if (accts[from].balance > 0)
		{
			payment = 1 + rand_range(accts[from].balance);
			accts[from].balance -= payment;
			accts[to].balance   += payment;
		}
		pthread_mutex_unlock(&accts[to].mtx);
		pthread_mutex_unlock(&accts[from].mtx);
	}
	return NULL;
}

int main(void)
{
	size_t i;
	long total;
	pthread_t ts[N_THREADS];

	srand(time(NULL));

	/* set the initial balance, but also create a
	   new mutex for each account */
	for (i = 0; i < N_ACCOUNTS; i++)
		accts[i] = (struct account)
			{100, PTHREAD_MUTEX_INITIALIZER};

	for (i = 0; i < N_THREADS; i++)
		pthread_create(&ts[i], NULL, disburse, NULL);

	puts("(This program will probably deadlock, "
	     "and need to be manually terminated...)");

	for (i = 0; i < N_THREADS; i++)
		pthread_join(ts[i], NULL);

	for (total = 0, i = 0; i < N_ACCOUNTS; i++)
		total += accts[i].balance;

	printf("Total money in system: %ld\n", total);
}