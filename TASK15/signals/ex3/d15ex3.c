#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
int k=0;
/*
void sig_handler(int sig_num, siginfo_t *info,void *args)
{
int * param= (int*) args;
printf("ОБРАБОТЧИК>получен сигнал SIGUSER1 - %d %d %d\n", sig_num, *param, info->si_signo);// пишут так  не стоит делать, может быть ошибка
k=1; // а вот флаг поднять стоит
}*/

int main(void)
{


    sigset_t set;
    int sig_num;
                                       
    sigemptyset( &set );
    sigaddset( &set, SIGUSR1 );
   if(0>sigprocmask( SIG_BLOCK, &set, NULL )) perror ("sigprocmask");

// int k=0;
 printf("Ожидаем сигнала SIGUSR1, мой pid=%d\n", getpid());
 while (1)
 {
  sigwait(&set,&sig_num);
  printf("Пришел сигнал \n");// секция eventLoop
  // есть еще  int eventfd(unsigned int initval, int flags);  на которой тоже можно сделать evenloop
 }

}
