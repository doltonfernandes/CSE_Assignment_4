#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
// #include <sys/shm.h>
// #include <unistd.h>
// #include <wait.h>

int R,D,S,maxWaitTime=5,cab[100];

pthread_t rider_t[100];

sem_t server[100],driver[100]; 

// sem_t mutex; 

// void* thread(void* arg) 
// { 
//     //wait 
//     sem_wait(&mutex); 
//     printf("\nEntered..\n"); 

//     //critical section 
//     sleep(4); 

//     //signal 
//     printf("\nJust Exiting...\n"); 
//     sem_post(&mutex); 
// } 


// int main()
// {
	// sem_init(&mutex,0,1); 
	// pthread_t t1,t2; 
	// pthread_create(&t1,NULL,thread,NULL); 
	// sleep(2); 
	// pthread_create(&t2,NULL,thread,NULL); 
	// pthread_join(t1,NULL); 
	// pthread_join(t2,NULL); 
	// sem_destroy(&mutex); 
// 	return 0; 
// }

void BookCab(int cabType,int maxwt,int RideTime)
{
	for(int i=0;;i++,i%=D)
	{
		if(!cab[i])
		{
			cab[i]=1;
			// pthread_create(&t1,NULL,thread,NULL); 
		}
	}
}

void *rider_thread(void *arg)
{
	BookCab(rand()%2,maxWaitTime,1+rand()%5);
	return NULL;
}

void runsimulation()
{
	srand(time(0));
	for(int i=0;i<S;i++)
	{
		sem_init(&server[i],0,1); 
	}
	for(int i=0;i<D;i++)
	{
		sem_init(&driver[i],0,1); 
	}
	for(int i=0;i<D;i++)
	{
		pthread_create(&rider_t[i],NULL,rider_thread,NULL); 
	}
	for(int i=0;i<S;i++)
	{
		sem_destroy(&server[i]); 
	}
	for(int i=0;i<D;i++)
	{
		sem_init(&driver[i],0,1); 
	}
}

int main(void)
{
	printf("Enter the Number of riders,Number of drivers and Number of servers\n");
	scanf("%d %d %d",&R,&D,&S);
	runsimulation();
	printf("Simulation Ended\n");
	return 0;
}