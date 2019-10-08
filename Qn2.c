#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/shm.h>
#include <unistd.h>
#include <wait.h>

struct vessel
{
	int r,p;
};

struct serving_table_details
{
	int table_no,slots;
};

pthread_mutex_t table[100];

int N,M,K,table_status[100],table_curr_slots[100],students_served;

pthread_t robot[100],student[100];

void *ready_to_serve_table(void *arg)
{
	struct serving_table_details *args=arg;
	table_curr_slots[args->table_no]-=args->slots;
	printf("Served %d from table %d\n",args->slots,args->table_no+1);
	return NULL;
}

void *serving_mode(void *arg)
{
	struct serving_table_details *args=arg,*tmp=malloc(sizeof(struct serving_table_details));
	tmp->table_no=args->table_no;
	while(table_curr_slots[args->table_no])
	{
		tmp->slots=1+rand()%(table_curr_slots[args->table_no]);
		pthread_t p;
		pthread_create(&p,NULL,ready_to_serve_table,tmp);
		pthread_join(p,NULL);
	}
	pthread_mutex_unlock(&table[args->table_no]);
	table_status[args->table_no]=0;
	return NULL;
}

void *biryani_ready(void *arg)
{
	struct vessel *v=arg;
	int i;
	for(i=0;v->r>0;i++,i%=N)
	{
		if(!table_status[i])
		{
			table_status[i]=1;
			pthread_mutex_lock(&table[i]);
			printf("%d capacity vessel given to table %d\n",v->p,i+1);
			table_curr_slots[i]=v->p;
			v->r--;
			struct serving_table_details *st=malloc(sizeof(struct serving_table_details));
			st->table_no=i;
			pthread_t *p=malloc(sizeof(pthread_t));
			pthread_create(p,NULL,serving_mode,st);
		}
	}
	return NULL;
}

void *robot_chef(void *arg)
{
	int w=2+rand()%4,r=1+rand()%10,p=25+rand()%26;
	printf("Preparing %d vessels of capacity %d in %d seconds\n",r,p,w);
	sleep(w);
	pthread_t pt;
	struct vessel *v=malloc(sizeof(struct vessel));
	v->r=r;
	v->p=p;
	pthread_create(&pt,NULL,biryani_ready,v);
	pthread_join(pt,NULL);
	return NULL;
}

void *wait_for_slot(void *arg)
{
	
	return NULL;
}

int main(void)
{
	scanf("%d %d %d",&N,&M,&K);
	for(int i=0;i<N;i++)
	{
	    pthread_mutex_init(&table[i],NULL);
	}
	srand(time(0));
	for(int i=0;i<M;i++)
	{
		pthread_create(&robot[i],NULL,robot_chef,NULL);
	}
	students_served=0;
	for(int i=0;i<K;i++)
	{
		pthread_create(&student[i],NULL,wait_for_slot,NULL);
	}
	while(students_served<K){}
	for(int i=0;i<N;i++)
	{
	    pthread_mutex_destroy(&table[i]);
	}
	return 0;
}