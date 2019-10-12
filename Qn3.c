#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

struct rider_details
{
	int id;
};

int R,D,S,driver_status[100];

pthread_t rider_t[100],driver_t[100],server_t[100];

pthread_mutex_t driver_m[100];

pthread_cond_t rider_c[100];

sem_t driver_s[100],free_cabs;

void *waiting_thread_1()
{
	return NULL;
}

int BookCab(int cabType,int maxWaitTime,int RideTime)
{
	struct timespec *wait=malloc(sizeof(struct timespec));
	wait->tv_sec=maxWaitTime;
	wait->tv_nsec=0;
	int x=sem_timedwait(&free_cabs,wait);
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
    if(cabType==0)
    {
    	return -1;
    }
	return 0;
}

void *rider_thread(void *arg)
{
	sleep(1+rand()%10);
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
	if(boooking_status>0)
	{
		printf("Rider %d booked cab %d successfully and ride has started\n",args->id+1,boooking_status);
		if(cabType)
		{
			pthread_mutex_lock(&driver_m[boooking_status-1]);
			if(driver_status[boooking_status-1]==0)
			{
				driver_status[boooking_status-1]=2;
	  	 		sem_post(&free_cabs); 
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
		  	 	sem_post(&free_cabs); 
			}
			else
			{
				driver_status[boooking_status-1]=0;
			}
			pthread_mutex_unlock(&driver_m[boooking_status-1]);
		}
		else
		{
			driver_status[boooking_status-1]=1;
			sleep(RideTime);
			printf("Rider %d done with ride in cab %d in %d seconds\n",args->id+1,boooking_status,RideTime);
			driver_status[boooking_status-1]=0;
	  	 	sem_post(&free_cabs); 
		}
		sem_post(&driver_s[boooking_status-1]);
	}
	else
	{
		printf("Rider %d couldn't book a ride due to wait timeout\n",args->id+1);
		if(boooking_status==-1)
		{
			sem_post(&free_cabs);
		}
	}
	return NULL;
}

void runsimulation()
{
	srand(time(0));
	sem_init(&free_cabs,0,D);
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
	sem_destroy(&free_cabs);
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
	runsimulation();
	return 0;
}