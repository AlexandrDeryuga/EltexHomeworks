#include <sys/msg.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Эта программа посылает сообщение в очередь */

#include <stdio.h>
#include <string.h>
#include <mqueue.h>

#define SIZE      10000

int main( int argc, char * argv){

 int prio=1;

  mqd_t ds;  //очередь posix
  char text[] = "Hi! Я - сервер.";
  char new_text[SIZE];
  
  struct mq_attr queue_attr;      // Атрибуты нашей очереди. Они могут быть установлены только при создании.
  queue_attr.mq_maxmsg = 32;     // максимальное число сообщений в очереди в один момент времени 
  queue_attr.mq_msgsize = SIZE; // максимальный размер очереди 

  printf("server POSIX - ожидаю клиента\n");
   //Создаём новую очередь и открываем её для отправки и приёма. 
   //Разрешения для файла очереди устанавливаем как rw для владельца и не разрешаем ничего
   //для группы/других. Ограничения очереди заданы указанными выше величинами.
   
  if ((ds = mq_open("/my_queue", O_CREAT | O_RDWR , 0600,  NULL )) == (mqd_t)-1){ //  имя начинается со слэша
   perror("open queue error");
   return -1;
  }

if(argc>1) { printf("argc=%d  закроем очередь\n",argc);
             if (mq_close(ds) == -1)  // закрываем очередь
              perror("Closing queue error");
               if (mq_unlink("/my_queue") == -1) perror("Removing queue error");
              return 0;
           }

 printf("server: идентификатор очереди# %d\n", (int) ds);
//Посылаем сообщение в очередь с приоритетом 2. Чем больше,число, тем выше приоритет. Сообщение с высоким приоритетом
//вставляется перед сообщением с низким приоритетом. Для сообщений с одинаковым приоритетом работает принцип
//первый вошёл, первый вышел.
   while (1==prio)     // ждем пока не придет сигнал с другим приоритетом
   {
   if (mq_send(ds, "wait", strlen("wait"), 1) == -1){
    perror("Sending message error");
    return -1;
     }
   sleep(1/3);  //3 раза в секунду опрашиваем
   if (mq_receive(ds, new_text, SIZE, &prio) == -1){
    perror("cannot receive");
    return -1;
     }
   
   }
   printf("server: соединение с клиентом установлено [%d]\n",prio);
   
 prio=3;
 while (3==prio)     // ждем пока не придет сигнал с другим приоритетом
   {
   if (mq_send(ds, text, strlen(text), 3) == -1){
    perror("Sending message error");
    return -1;
     }
   sleep(1/3);  //3 раза в секунду опрашиваем
   
   //Принимаем сообщение из очереди. Это блокирующий вызов.
   //Приоритет принятого сообщения сохраняется в prio. Функция
   //принимает самое старое из сообщений с наивысшим приоритетом
   //из очереди сообщений. Если размер буфера, указанный аргументом
   //msg_len, меньше, чем атрибут mq_msgsize очереди сообщений,
   //вызов функции не будет успешным и вернёт ошибку.  
    
     if (mq_receive(ds, new_text, SIZE, &prio) == -1){
    perror("cannot receive");
    return -1;
     }
  
   }
 
 
  
    printf("server: Получили сообщение от клиента: [%s], приоритет = %d\n", new_text, prio);


  if (mq_close(ds) == -1)  // закрываем очередь
    perror("Closing queue error");
    if (mq_unlink("/my_queue") == -1)

    perror("Removing queue error");

  return 0;

}

