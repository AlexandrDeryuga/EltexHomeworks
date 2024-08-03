#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include <unistd.h>
#include <sys/syscall.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>


#include <pthread.h>
//#include <thread.h>

#define NUM_THREADS 5
#define SLEEP_TIME 1
#define _COUNTER 90000000

typedef  struct data
    {long int a;
     long int sumb;
     long int from;
     long int to;
    } data;

void *MyThreadFunc(data *);   /* thread  */
int i;
pthread_t tid[NUM_THREADS];      /* array of thread IDs */

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

int main()
{
    unsigned long id;
    data x;
    
    
    x.a=0, x.sumb=0; x.from=0;x.to=_COUNTER/NUM_THREADS;
    errno = 0;
    id = syscall(SYS_gettid);
    if(0 != errno)
    {
        perror("syscall(SYS_gettid) failed");
        abort();
    }
    //fprintf(stdout, "thread id %ld", id);
    printf("Главная thread id (gettid) %lu - создаю потоки!\n", id);
     
    clock_t start, end;
    long double elapsed;
    start = clock();
                             
                             //end = clock();
                           //  elapsed=((double)(end-start))/CLOCKS_PER_SEC;


    
    
    //printf("Date: %d.%d.%d\n", now->tm_mday, now->tm_mon + 1, now->tm_year + 1900);
    //printf("Time: %d:%d:%d\n", now->tm_hour, now->tm_min, now->tm_sec);
    
    
        for ( i = 0; i < NUM_THREADS; i++)
            {
            pthread_create(&tid[i], NULL, MyThreadFunc,&x);  // Создадим поток
            x.from=x.to;x.to=x.to+_COUNTER/NUM_THREADS;
            }
        for ( i = 0; i < NUM_THREADS; i++)
            pthread_join(tid[i], NULL);
        
        end = clock();
        elapsed=((double)(end-start))/CLOCKS_PER_SEC;;
        
        printf("Все %d нитей завершились, a=%ld  sumb=%ld , время работы: %Lf  сек.\n", i,x.a,x.sumb,elapsed);
         
                             

    
    exit(0);
}



void *MyThreadFunc(data *x)
{
    unsigned long id;
   unsigned long b=0,c=0;
   
   
   
   
   
    errno = 0;
    id = syscall(SYS_gettid);
    if(0 != errno)   { perror("syscall(SYS_gettid) failed");abort();}
    //printf("поток %lu> аргумент a=%d\n", id, c);
   c=_COUNTER/NUM_THREADS;
    while (b<c) 
       {
        
       (x->a)++;b++;
      
       // pthread_mutex_lock( &mutex ); 
       (x->a)++;b++;                          // оставим вычислительную нагрузку ту же
       //pthread_mutex_unlock( &mutex );
        };
   
   printf("поток %lu> цикл выполнилcя b=%ld раз\n", id, b);
    pthread_mutex_lock( &mutex ); 
    x->sumb=x->sumb+b;
    pthread_mutex_unlock( &mutex );
    
}
