#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

struct rider_details
{
	int id;
};

int R,D,S,driver_status[100],cabs_are_free,pools_degree2_free=0;

pthread_t rider_t[100],driver_t[100],server_t[100];

pthread_mutex_t driver_m[100],free_cabs;

pthread_cond_t rider_c[100],free_cab_c;

sem_t driver_s[100];

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

int BookCab(int cabType,int maxWaitTime,int RideTime)
{
	struct timespec *wait=malloc(sizeof(struct timespec));
	wait->tv_sec=maxWaitTime;
	wait->tv_nsec=0;
	int x=pthread_mutex_timedlock(&free_cabs,wait);
	pthread_mutex_unlock(&free_cabs);
	if(x!=0)
	{
		return 0;
	}
    for(int i=0;i<D;i++)
    {
    	if(cabType)
    	{
    		if(driver_status[i]==0 || driver_status[i]==2)
    		{
    			sem_wait(&driver_s[i]);
	    		if(!(driver_status[i]==0 || driver_status[i]==2))
	    		{
	    			sem_post(&driver_s[i]); 
				    continue;
	    		}
	    		return i+1;
    		}
    	}
		else
		{
			if(driver_status[i]==0)
			{
    			sem_wait(&driver_s[i]); 
    			if(driver_status[i])
    			{
				    sem_post(&driver_s[i]); 
				    continue;
    			}
    			return i+1; 
			}
		}
    }
	return 0;
}

void *rider_thread(void *arg)
{
	sleep(1+rand()%4);
	int cabType=rand()%2;
	int maxWaitTime=1+rand()%10;
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
	int boooking_status=BookCab(cabType,maxWaitTime,RideTime);
	if(boooking_status)
	{
		printf("Rider %d booked cab %d successfully and ride has started\n",args->id+1,boooking_status);
		if(cabType)
		{
			pthread_mutex_lock(&driver_m[boooking_status-1]);
			if(driver_status[boooking_status-1]==0)
			{
				driver_status[boooking_status-1]=2;
				pthread_mutex_unlock(&driver_m[boooking_status-1]);
				sleep(RideTime);
				printf("Rider %d done with ride in cab %d in %d seconds\n",args->id+1,boooking_status,RideTime);
			}
			else
			{
				driver_status[boooking_status-1]=3;
				pthread_mutex_unlock(&driver_m[boooking_status-1]);
				sleep(RideTime);
				printf("Rider %d done with ride in cab %d in %d seconds\n",args->id+1,boooking_status,RideTime);
			}
			pthread_mutex_lock(&driver_m[boooking_status-1]);
			if(driver_status[boooking_status-1]==3)
			{
				driver_status[boooking_status-1]=2;
			}
			else
			{
				driver_status[boooking_status-1]=0;
			}
			pthread_mutex_unlock(&driver_m[boooking_status-1]);
		    sem_post(&driver_s[boooking_status-1]);
		}
		else
		{
			driver_status[boooking_status-1]=1;
			sleep(RideTime);
			printf("Rider %d done with ride in cab %d in %d seconds\n",args->id+1,boooking_status,RideTime);
			driver_status[boooking_status-1]=0;
		    sem_post(&driver_s[boooking_status-1]);
		}
	}
	else
	{
		printf("Rider %d couldn't book a ride due to wait timeout\n",args->id+1);
	}
	return NULL;
}

void runsimulation()
{
	srand(time(0));
	pthread_mutex_init(&free_cabs,NULL);
    pthread_cond_init(&free_cab_c,NULL);
	for(int i=0;i<R;i++)
	{
		pthread_cond_init(&rider_c[i],NULL);
	}
	for(int i=0;i<D;i++)
	{
		sem_init(&driver_s[i],0,2); 
	}
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
	pthread_mutex_destroy(&free_cabs);
    pthread_cond_destroy(&free_cab_c);
	for(int i=0;i<R;i++)
	{
		pthread_cond_destroy(&rider_c[i]);
	}
	for(int i=0;i<D;i++)
	{
		sem_destroy(&driver_s[i]); 
	}
}

int main(void)
{
	printf("Enter the Number of riders,Number of drivers and Number of servers\n");
	scanf("%d %d %d",&R,&D,&S);
	cabs_are_free=D;
	runsimulation();
	return 0;
}