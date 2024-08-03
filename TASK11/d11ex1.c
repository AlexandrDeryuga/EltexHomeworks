#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include <unistd.h>
#include <sys/syscall.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>


#include <pthread.h>
//#include <thread.h>

#define NUM_THREADS 5
#define SLEEP_TIME 1

void *MyThreadFunc();   /* thread  */
int i;
pthread_t tid[NUM_THREADS];      /* array of thread IDs */



int main()
{
    unsigned long id;
    errno = 0;
    id = syscall(SYS_gettid);
    if(0 != errno)
    {
        perror("syscall(SYS_gettid) failed");
        abort();
    }
    //fprintf(stdout, "thread id %ld", id);
    printf("Главная thread id (gettid) %lu - создаю потоки!\n", id);
    
    
        for ( i = 0; i < NUM_THREADS; i++)
            pthread_create(&tid[i], NULL, MyThreadFunc,NULL);  // Создадим поток
        for ( i = 0; i < NUM_THREADS; i++)
            pthread_join(tid[i], NULL);
            
        printf("Все %d нитей завершились\n", i);
    
    exit(0);
}



void * MyThreadFunc()
{
    
    unsigned long id;
    errno = 0;
    id = syscall(SYS_gettid);
    if(0 != errno)
    {
        perror("syscall(SYS_gettid) failed");
        abort();
    }
    //fprintf(stdout, "thread id %li ", id);
    printf("thread id:  (gettid) %lu      (pthread_self) %lu\n", id, pthread_self());
    
  
    printf("thread  (getid) %lu ушла в сон \n", id);
    sleep(SLEEP_TIME);
    printf("\nthread (getid) %lu проснулась\n", id);
    return (NULL);
}
