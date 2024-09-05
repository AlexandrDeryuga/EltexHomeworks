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
struct sigaction handler={0};
sigset_t set;
int ret;

sigemptyset(&set);
sigaddset(&set,SIGUSR1);
handler.sa_sigaction = sig_handler;
//handler.sa_mask= set;

ret=sigaction(SIGUSR1,&handler,NULL);
if(ret<0){ 
   perror("Can't set signal handler!\n");
   exit(EXIT_FAILURE);
 }
// int k=0;
 printf("Ожидаем сигнала, мой pid=%d\n", getpid());
 while (1)
 {
 sleep(1);
 if(k) {printf("MAIN>получен сигнал SIGUSER1\n");k=0;}  
 //предлагается использовать такого рода конструкции  - обработчик подымает флаг, обрабатываем событие в самой программе
 }

}
