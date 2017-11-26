#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <math.h>
#include <time.h>


typedef struct task {
   int i;
   char* dst;
   int minThinkingTime;
   int maxThinkingTime;
   int minDiningTime;
   int maxDiningTime;
   int numberOfDinners;
   int numberOfPhilosophers;
} workerTask;

sem_t* forks;
sem_t* dinnerQuotaLock;

int uniformDistribution(int rangeLow, int rangeHigh) {
    double myRand = rand()/(1.0 + RAND_MAX); 
    int range = rangeHigh - rangeLow + 1;
    int myRand_scaled = (myRand * range) + rangeLow;
    return myRand_scaled;
}

double exponentialDistribution(double lambda)
{
  double z;                     // Uniform random number (0 < z < 1)
  double expValue;             // Computed exponential value to be returned

  z = (double) uniformDistribution(1, 100) / 101.0;

  // Compute exponential random variable using inversion method
  expValue = -lambda * log(z);
  return (int) expValue;
}

void workerThread(void *i) {

    workerTask *a = ((workerTask *) i);
	int thinkingTime = 0;
	int diningTime = 0;

	if(a->dst == "uniform") {
		thinkingTime = uniformDistribution(a->minThinkingTime, a->maxThinkingTime);
		diningTime = uniformDistribution(a->minDiningTime, a->maxDiningTime);
	} else {
		thinkingTime = exponentialDistribution((double) (a->minThinkingTime + a->maxThinkingTime) / 2);
		diningTime = exponentialDistribution( (double) ((a->minDiningTime + a->maxDiningTime) / 2));
	}

	while(a->numberOfDinners > 0) {
		usleep(thinkingTime*1000);


		sem_wait(&dinnerQuotaLock);
		sem_wait(&forks[a->i]);
		sem_wait(&forks[(a->i+1)%a->numberOfPhilosophers]);

		a->numberOfDinners--;

		usleep(diningTime*1000);

		sem_post(&forks[a->i]);
		sem_post(&forks[(a->i+1)%a->numberOfPhilosophers]);
		sem_post(&dinnerQuotaLock);
	}

    printf("philosopher #%d finished its job\n", a->i);
    free(a);
    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {

	/* will get commented */
	//char* a[8] = {"a", "5", "100", "3000", "200", "2000", "exponential", "5"};
    //argv = a;
    /* will get commented */


	pid_t pid;
	int i;

	int numberOfPhilosophers = atoi(argv[1]);
	int minThinkingTime = atoi(argv[2]);
	int maxThinkingTime = atoi(argv[3]);
	int minDiningTime = atoi(argv[4]);
	int maxDiningTime = atoi(argv[5]);
	char* probDist = argv[6];
	int numberOfDinners = atoi(argv[7]);
	int tid[numberOfPhilosophers];

	sem_t s[numberOfPhilosophers];
    forks = s;
	
    sem_init(&dinnerQuotaLock, 0, numberOfPhilosophers / 2);

	for (i = 0; i <= numberOfPhilosophers; i++) {
		sem_init(&forks[i], 0, 1);
	}

	/* Start threads. */

	for (i = 0; i < numberOfPhilosophers; ++i) {

        workerTask* arg = (workerTask * ) malloc(sizeof(workerTask));

        arg->dst = probDist;
        arg->maxDiningTime = maxDiningTime;
        arg->maxThinkingTime = maxThinkingTime;
        arg->minDiningTime = minDiningTime;
        arg->minThinkingTime = minThinkingTime;
        arg->numberOfDinners = numberOfDinners;
        arg->numberOfPhilosophers = numberOfPhilosophers;
        arg->i = i;
        pthread_create(&tid[i], NULL, &workerThread, (void *)arg);
    }

	pthread_exit(NULL);
}

