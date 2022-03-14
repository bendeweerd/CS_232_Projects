/* banker.c */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

#define N_ACCOUNTS 10
#define N_THREADS  20
#define N_ROUNDS   10000

/* 10 accounts with $100 apiece means there's $1,000
   in the system. Let's hope it stays that way...  */
#define INIT_BALANCE 100

/* making a struct here for the benefit of future
   versions of this program */
struct account
{
	long balance;
} accts[N_ACCOUNTS];

/* Helper for bankers to choose an account and amount at
   random. It came from Steve Summit's excellent C FAQ
   http://c-faq.com/lib/randrange.html */
int rand_range(int N)
{
	return (int)((double)rand() / ((double)RAND_MAX + 1) * N);
}

/* each banker will run this function concurrently. The
   weird signature is required for a thread function */
void *disburse(void *arg)
{
	size_t i, from, to;
	long payment;

	/* idiom to tell compiler arg is unused */
	(void)arg;

	for (i = 0; i < N_ROUNDS; i++)
	{
		/* pick distinct 'from' and 'to' accounts */
		from = rand_range(N_ACCOUNTS);
		do {
			to = rand_range(N_ACCOUNTS);
		} while (to == from);

		/* go nuts sending money, try not to overdraft */
		if (accts[from].balance > 0)
		{
			payment = 1 + rand_range(accts[from].balance);
			accts[from].balance -= payment;
			accts[to].balance   += payment;
		}
	}
	return NULL;
}

int main(void)
{
	size_t i;
	long total;
	pthread_t ts[N_THREADS];

	srand(time(NULL));

	for (i = 0; i < N_ACCOUNTS; i++)
		accts[i].balance = INIT_BALANCE;

	printf("Initial money in system: %d\n",
		N_ACCOUNTS * INIT_BALANCE);

	/* start the threads, using whatever parallelism the
	   system happens to offer. Note that pthread_create
	   is the *only* function that creates concurrency */
	for (i = 0; i < N_THREADS; i++)
		pthread_create(&ts[i], NULL, disburse, NULL);

	/* wait for the threads to all finish, using the
	   pthread_t handles pthread_create gave us */
	for (i = 0; i < N_THREADS; i++)
		pthread_join(ts[i], NULL);

	for (total = 0, i = 0; i < N_ACCOUNTS; i++)
		total += accts[i].balance;

	printf("Final money in system: %ld\n", total);
}