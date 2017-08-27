#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>


//to compile in intel processor gcc -pthread <filename.c>
// to compile on amd processor gcc -lpthread <filename.c>


static int counter =0; //global variable

void *functionthreads( void * tID)
{	
	int i;
	//extern int counter;
	long threadID;
	threadID = (long)tID;
	printf("Hello World ! It's me , thread #%ld!\t with threadID #%ld\n", threadID, pthread_self());
	for(i=0;i<1000;i++)
	counter++;
	pthread_exit(NULL);
}


int main(int argc , char* argv[])
{
	pthread_t t[5]; //creating objects for arrays of threads
	char *message1 = "Message1";
	char *message2 = "message2";
	int errorflag ;
	long i;
	
	//creating indepenent threads each will call functionthreads 
	for(i=0;i<5;i++)
	{
		printf("IN Main(): Creating thread %ld\n", i);
		errorflag = pthread_create (&t[i], NULL , &functionthreads, (void *)i );
		
		/*
		arguments of pthread_create( first is address of object of type pthread_t
		, then some thread attribute usually NULL, address of function to be executed by thread,
		arguments needed for that function call need to be (void*) type )	
		*/
		if(errorflag<0)
		{	printf("Error in creating thread number :%ld", i);
			exit(-1);
		}		
	}
	
	for(i=0;i<5;i++)
	{
		errorflag = pthread_join(t[i],NULL);
		if(errorflag<0)
		{	printf("Error in Joining thread number :%ld", i);
			exit(-1);
		}
	}
	
	//Lets see whats in pthread_t t[5]
	for(i=0;i<5;i++)
	{
		printf("\n most probably thread id of thread[%ld]: %ld", i ,  (long*)t[i]);
	}
	
	//lets see whats happened to global variable 
	
	printf("\n\n\nFINAL VALUE OF GLOBAL VARIABLE :%d\n", counter);
	
}
