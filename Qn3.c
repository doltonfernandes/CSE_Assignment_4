#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

struct rider_details
{
	int id;
};

int R,D,S;

pthread_t rider_t[100],driver_t[100],server_t[100];

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

void BookCab(int cabType,int maxWaitTime,int RideTime)
{
	
}

void *rider_thread(void *arg)
{
	sleep(1+rand()%4);
	int cabType=rand()%2;
	int maxWaitTime=1+rand()%6;
	int RideTime=1+rand()%5;
	struct rider_details *args=arg;
	if(cabType)
	{
		printf("Rider %d arrived who is looking for Pool Ride\n",args->id+1);
	}
	else
	{
		printf("Rider %d arrived who is looking for Premier Ride\n",args->id+1);
	}
	BookCab(cabType,maxWaitTime,RideTime);
	return NULL;
}

void runsimulation()
{
	srand(time(0));
	for(int i=0;i<R;i++)
	{
		struct rider_details *p=malloc(sizeof(struct rider_details));
		p->id=i;
		pthread_create(&rider_t[i],NULL,rider_thread,p); 
	}
	for(int i=0;i<R;i++)
	{
		pthread_join(rider_t[i],NULL); 
	}
	printf("Simulation Ended\n");
}

int main(void)
{
	printf("Enter the Number of riders,Number of drivers and Number of servers\n");
	scanf("%d %d %d",&R,&D,&S);
	runsimulation();
	return 0;
}