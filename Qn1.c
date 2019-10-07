#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>

struct data
{
	int *arr,l,r,pivot;
};

void swap(int *a,int *b)
{
	int tmp=*a;
	*a=*b;
	*b=tmp;
}

int partition(int *arr,int l,int r,int pivot)
{
	int i=l-1;
	for(int j=l;j<r;j++)
	{
		if(arr[j]<=pivot)
		{
			i++;
			swap(&arr[i],&arr[j]);
		}
	}
	i++;
	swap(&arr[i],&arr[r]);
	return i;
}

int thread_cnt,thread_max=10;

void *quicksort(void *arg)
{
	struct data *args=arg;
	int *arr=args->arr,l=args->l,r=args->r,pivot=args->pivot;
	if(l<r)
	{
		int pos=partition(arr,l,r,pivot);
		struct data *args=malloc(sizeof(struct data));
		args->arr=arr;
		args->l=l;
		args->r=pos-1;
		args->pivot=arr[pos-1];
		quicksort(args);
		args->l=pos+1;
		args->r=r;
		args->pivot=arr[r];
		if(thread_cnt<thread_max)
		{
			pthread_t p1;
			thread_cnt++;
			pthread_create(&p1,NULL,quicksort,args);
			pthread_join(p1,NULL);
			thread_cnt--;
		}
		else
		{
			quicksort(args);
		}
	}
	return NULL;
}

int main(void)
{
	int arr[100004],n;
	scanf("%d",&n);
	for(int i=0;i<n;i++)
	{
		scanf("%d",&arr[i]);
	}
	pthread_t p1;
	struct data *args=malloc(sizeof(struct data));
	args->arr=arr;
	args->l=0;
	args->r=n-1;
	args->pivot=arr[n-1];
	thread_cnt=1;
	pthread_create(&p1,NULL,quicksort,args);
	pthread_join(p1,NULL);
	thread_cnt--;
	for(int i=0;i<n;i++)
	{
		printf("%d ",arr[i]);
	}
	printf("\n");
	return 0;
}