#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <sys/types.h>


/*
    Системный вызов kill() предназначен для передачи сигнала одному или нескольким специфицированным процессам в рамках полномочий пользователя.
    Послать сигнал (если у вас нет полномочий суперпользователя) можно только процессу, у которого эффективный идентификатор пользователя совпадает с эффективным идентификатором пользователя для процесса, посылающего сигнал.
    Аргумент pid описывает, кому посылается сигнал, а аргумент sig — какой сигнал посылается. Этот системный вызов умеет делать много разных вещей, в зависимости от значения аргументов:
        Если pid > 0 и sig > 0, то сигнал номером sig (если позволяют привилегии) посылается процессу с идентификатором pid.
        Если pid = 0, а sig > 0, то сигнал с номером sig посылается всем процессам в группе, к которой принадлежит посылающий процесс.
        Если pid = -1, sig > 0 и посылающий процесс не является процессом суперпользователя, то сигнал посылается всем процессам в системе, для которых идентификатор пользователя совпадает с эффективным идентификатором пользователя процесса, посылающего сигнал.
        Если pid = -1, sig > 0 и посылающий процесс является процессом суперпользователя, то сигнал посылается всем процессам в системе, за исключением системных процессов (обычно всем, кроме процессов с pid = 0 и pid = 1).
        Если pid < 0, но не –1, sig > 0, то сигнал посылается всем процессам из группы, идентификатор которой равен абсолютному значению аргумента pid (если позволяют привилегии).
        Если значение sig = 0, то производится проверка на ошибку, а сигнал не посылается, так как все сигналы имеют номера > 0. Это можно использовать для проверки правильности аргумента pid ( есть ли в системе процесс или группа процессов с соответствующим идентификатором).

    Системный вызов возвращает 0 при нормальном завершении и –1 при ошибке.
*/


void main()
{
//int kill(pid_t pid, int signal);
int k=1;int pid;
while (k){
printf("введите pid процесса, которому нужно послать сигнал\n");
scanf("%d", &pid);
printf("посылаем сигнал SIGINT процессу %d\n",pid);

if(!kill(pid,0)) {kill(pid,SIGINT);k=0;} else {printf("нет такого процесса\n");};
}
}
