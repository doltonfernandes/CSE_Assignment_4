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

struct student_details
{
	int id;
};

int min(int a,int b)
{
	if(a<b)
	{
		return a;
	}
	return b;
}

pthread_mutex_t table[100],queue[100],queue_zero_m[100],student_m[100],main_mut;

pthread_cond_t queue_zero_c[100],student_c[100];

int N,M,K,table_status[100],table_curr_slots[100],queue_a[100][100],student_status[100],students_served=0;

pthread_t robot[100],student[100];

void *ready_to_serve_table(void *arg)
{
	struct serving_table_details *args=arg;
	printf("Serving biryani after %d student(s) come at table %d\n",args->slots,args->table_no+1);
	int size=0,stak[100];
	for(int i=0;size<args->slots;i++)
	{
		if(student_status[i]==0)
		{
			pthread_mutex_lock(&student_m[i]);
			if(student_status[i]==0)
			{
				student_status[i]=args->table_no+1;
				sleep(0.5);
				pthread_cond_signal(&student_c[i]);
				stak[size++]=i;
			}
			pthread_mutex_unlock(&student_m[i]);
		}
	}
	sleep(0.5);
	printf("Started serving biryani at table %d\n",args->table_no+1);
	for(int i=0;i<size;i++)
	{
		sleep(0.5);
		printf("Biryani served to student with id - %d from table %d\n",stak[i]+1,args->table_no+1);
	}
	printf("Served %d slot(s) of biryani from table %d\n",args->slots,args->table_no+1);
	pthread_mutex_lock(&queue_zero_m[args->table_no]);
	table_curr_slots[args->table_no]-=args->slots;
	pthread_cond_broadcast(&queue_zero_c[args->table_no]);
	pthread_mutex_unlock(&queue_zero_m[args->table_no]);
	return NULL;
}

void *serving_mode(void *arg)
{
	struct serving_table_details *args=arg,*tmp=malloc(sizeof(struct serving_table_details));
	tmp->table_no=args->table_no;
	while(table_curr_slots[args->table_no])
	{
		pthread_mutex_lock(&main_mut);
		if(students_served==K)
		{
			pthread_mutex_unlock(&main_mut);
			break;
		}
		tmp->slots=min(1+rand()%(K-students_served),1+rand()%(table_curr_slots[args->table_no]));
		students_served+=tmp->slots;
		pthread_mutex_unlock(&main_mut);
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
	for(int i=0;v->r>0 && students_served<K;i++,i%=N)
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
	struct student_details *args=arg;
	pthread_mutex_lock(&student_m[args->id]);
	pthread_cond_wait(&student_c[args->id],&student_m[args->id]);
	printf("Student with id %d arrived at table %d\n",args->id+1,student_status[args->id]);
	pthread_mutex_unlock(&student_m[args->id]);
	return NULL;
}

void *student_t(void *arg)
{
	struct student_details *args=arg;
	wait_for_slot(arg);
	pthread_mutex_lock(&queue_zero_m[student_status[args->id]-1]);
	pthread_cond_wait(&queue_zero_c[student_status[args->id]-1],&queue_zero_m[student_status[args->id]-1]);
	pthread_mutex_unlock(&queue_zero_m[student_status[args->id]-1]);
	return NULL;
}

int main(void)
{
	printf("Enter the Number of tables,Number of robots and Number of students\n");
	scanf("%d %d %d",&N,&M,&K);
	pthread_mutex_init(&main_mut,NULL);
	for(int i=0;i<N;i++)
	{
	    pthread_mutex_init(&table[i],NULL);
	    pthread_mutex_init(&queue[i],NULL);
	    pthread_mutex_init(&queue_zero_m[i],NULL);
	    pthread_cond_init(&queue_zero_c[i],NULL);
	    pthread_mutex_init(&queue_zero_m[i],NULL);
	    pthread_cond_init(&queue_zero_c[i],NULL);
	}
	for(int i=0;i<K;i++)
	{
		pthread_mutex_init(&student_m[i],NULL);
	    pthread_cond_init(&student_c[i],NULL);
	}
	srand(time(0));
	for(int i=0;i<M;i++)
	{
		pthread_create(&robot[i],NULL,robot_chef,NULL);
	}
	for(int i=0;i<K;i++)
	{
		struct student_details *s=malloc(sizeof(struct student_details));
		s->id=i;
		pthread_create(&student[i],NULL,student_t,s);
	}
	for(int i=0;i<K;i++)
	{
		pthread_join(student[i],NULL);
	}
	printf("All %d students served biryani successfully\n",K);
	pthread_mutex_destroy(&main_mut);
	for(int i=0;i<N;i++)
	{
	    pthread_mutex_destroy(&table[i]);
	    pthread_mutex_destroy(&queue[i]);
	    pthread_mutex_destroy(&queue_zero_m[i]);
	    pthread_cond_destroy(&queue_zero_c[i]);
	}
	for(int i=0;i<K;i++)
	{
		pthread_mutex_destroy(&student_m[i]);
	    pthread_cond_destroy(&student_c[i]);
	}
	return 0;
}