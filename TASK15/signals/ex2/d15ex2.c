#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
int k=0;

void sig_handler(int sig_num, siginfo_t *info,void *args)
{
int * param= (int*) args;
printf("ОБРАБОТЧИК>получен сигнал SIGUSER1 - %d %d %d\n", sig_num, *param, info->si_signo);// пишут так  не стоит делать, может быть ошибка
k=1; // а вот флаг поднять стоит
}

int main(void)
{


    sigset_t set, oset, pset;
                                       
    sigemptyset( &set );
    sigaddset( &set, SIGINT );
   if(!sigprocmask( SIG_BLOCK, &set, &oset )) perror ("sigprocmask");

// int k=0;
 printf("Ожидаем сигнала SIGINT но он будет проигнорирован, мой pid=%d\n", getpid());
 while (1)
 {
 sleep(1);
 if(k) {printf("MAIN>получен сигнал SIGUSER1\n");k=0;}  
 //предлагается использовать такого рода конструкции  - обработчик подымает флаг, обрабатываем событие в самой программе
 }

}
