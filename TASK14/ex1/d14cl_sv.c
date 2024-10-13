//Клиент

#include <stdio.h>
#include <string.h>
#include "message.h"


int main ()
{
  int semid;                    // идентификатор семафора 
  int shmid;                    // идентификатор разделяемой памяти 
  message_t *msg_p;             // адрес сообщения в разделяемой памяти 
  char s[MAX_STRING];

  printf("shmat client started..\n");
  // получение доступа к массиву семафоров 
  if ((semid = semget (SEM_ID, 1, 0)) < 0)
    perror ("client: can not get semaphore");

  // получение доступа к сегменту разделяемой памяти 
  if ((shmid = shmget (SHM_ID, sizeof (message_t), 0)) < 0)
    perror ("client: can not get shared memory segment");

  // получение адреса сегмента 
  if ((msg_p = (message_t *) shmat (shmid, 0, 0)) == NULL)
    perror ("client: shared memory attach error");

      
      strncpy (msg_p->string, "Hi!", MAX_STRING); // конструкция с контролем   - запишем строку и флаг доступности в память
      msg_p->type = MSG_TYPE_STRING;
      while (semctl (semid, 0, GETVAL, 0) || msg_p->type != MSG_TYPE_EMPTY) { semctl (semid, 0, SETVAL, 0);}; //  если сообщение не обработано или сегмент блокирован - ждать
       
      semctl (semid, 0, SETVAL, 1);     //Блокировать
      
      msg_p->type = MSG_TYPE_FINISH;
    
   

      semctl (semid, 0, SETVAL, 0);     // отменить блокировку 


  shmdt (msg_p);                // отсоединить сегмент разделяемой памяти 
    
  exit (0);
}
