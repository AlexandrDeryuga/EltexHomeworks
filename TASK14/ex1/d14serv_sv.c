///////////////////////////      СЕРВЕР , разделяемая память

#include <stdio.h>
#include <string.h>
#include "message.h"

int main ()
{
  int semid;                    // идентификатор семафора 
  int shmid;                    // идентификатор разделяемой памяти 
  message_t *msg_p;             // адрес сообщения в разделяемой памяти
  char s[MAX_STRING];
 printf("shmat server started..\n");
                                                             //создание массива семафоров из одного элемента
  if ((semid = semget (SEM_ID, 1, PERMS | IPC_CREAT)) < 0)
    perror ("server: can not create semaphore");

                                                              // создание сегмента разделяемой памяти 
  if ((shmid = shmget (SHM_ID, sizeof (message_t), PERMS | IPC_CREAT)) < 0)
    perror ("server: can not create shared memory segment");

                                                             // подключение сегмента к адресному пространству процесса 
  if ((msg_p = (message_t *) shmat (shmid, 0, 0)) == NULL)
    perror ("server: shared memory attach error");

  semctl (semid, 0, SETVAL, 0); // установка семафора   int semctl (int semid, int semnum, int cmd, arg);    
  /* 
     В качестве аргумента semid должен выступать идентификатор множества семафоров, предварительно полученный при помощи системного вызова semget. 
     Аргумент semnum задает номер семафора во множестве. Семафоры нумеруются с нуля. 
    
    GETVAL - получить значение семафора и выдать его в качестве результата;
    SETVAL - установить значение семафора равным arg.val;
    GETPID - получить идентификатор процесса, выполнявшего операцию над семафором последним , и выдать его в качестве результата;
    GETNCNT - получить число процессов, ожидающих увеличения значения семафора, и выдать его в качестве результата;
    GETZCNT - получить число процессов, ожидающих обнуления значения семафора, и выдать его в качестве результата;
    GETALL - прочитать значения всех семафоров множества и поместить их в массив, на который указывает arg.array;
    SETALL - установить значения всех семафоров множества равными значениям элементов массива, на который указывает
    arg.array;
    IPC_STAT - поместить информацию о состоянии множества семафоров, содержащуюся в структуре данных, ассоциированной с идентификатором semid, в пользовательскую структуру, на которую указывает arg.buf;
    IPC_SET - в структуре данных, ассоциированной с идентификатором semid, переустановить значения действующих идентификаторов пользователя и группы, а также прав на операции. Нужные значения извлекаются из структуры данных, на которую указывает arg.buf;
    IPC_RMID - удалить из системы идентификатор semid, ликвидировать множество семафоров и ассоциированную с ним структуру данных.
    
     union semun {               //структура аргументов
     int val;
     struct semid_ds *buf;
     ushort *array;
     } arg; 
*/
  
  
  msg_p->type = MSG_TYPE_EMPTY;
   //msg_p->type =MSG_TYPE_FINISH;
 
      while (msg_p->type != MSG_TYPE_FINISH)
        {
          if (semctl (semid, 0, GETVAL, 0))     // блокировка - ждать 
            continue;

          //semctl (semid, 0, SETVAL, 1); // установить блокировку 

          // обработка сообщения 
           
        
          msg_p->type = MSG_TYPE_EMPTY; // сообщение обработано 
          //semctl (semid, 0, SETVAL, 0); // снять блокировку 
          sleep(0.3);
      }
       printf ("%s\n", msg_p->string);
    

  /* удаление массива семафоров */
  if (semctl (semid, 0, IPC_RMID, (struct semid_ds *) 0) < 0)
    perror ("server: semaphore remove error");

  /* удаление сегмента разделяемой памяти */
  shmdt (msg_p);
  if (shmctl (shmid, IPC_RMID, (struct shmid_ds *) 0) < 0)
    perror("server: shared memory remove error");
    
  exit (0);
}
