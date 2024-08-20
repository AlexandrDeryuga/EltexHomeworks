#include <stdio.h>
#include <mqueue.h>
#include <sys/msg.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SIZE       10000

 
int main(){

mqd_t ds;
char new_text[SIZE];
char text[] = "Hello! Я - клиент.";

struct mq_attr attr;

int prio=2;

     //Открываем "/my_queue" для отправки и приёма. При приёме
    //сообщения могут не блокироваться если указать O_RDWR | O_NONBLOCK 
   //Разрешения для файла очереди устанавливаем как rw для владельца и не разрешаем
  //ничего для группы/других.

  printf("client POSIX - ожидаю сервер\n");
  
  while ((ds = mq_open("/my_queue", O_RDWR, 0600, NULL)) == (mqd_t)-1){
    perror("client: Сервер не запущен, или ошибка очереди..\n");
    sleep(2);
    //return -1;
  }
  printf("client: идентификатор очереди# %d\n" ,(int) ds);
  
   while (2==prio)     // ждем пока не придет сигнал с другим приоритетом
   {
   if (mq_send(ds, "wait", strlen("wait"), 2) == -1){
    perror("Sending message error");
    return -1;
     }
   sleep(1/3);  //3 раза в секунду опрашиваем
   if (mq_receive(ds, new_text, SIZE, &prio) == -1){
    perror("cannot receive");
    return -1;
     } 
   }
  
   printf("client: соединение с сервером установлено [%d]\n",prio);


 
   //Принимаем сообщение из очереди. Это блокирующий вызов.
   //Приоритет принятого сообщения сохраняется в prio. Функция
   //принимает самое старое из сообщений с наивысшим приоритетом
   //из очереди сообщений. Если размер буфера, указанный аргументом
   //msg_len, меньше, чем атрибут mq_msgsize очереди сообщений,
   //вызов функции не будет успешным и вернёт ошибку.


while (prio!=3){    //проверим, не осталось ли в очереди мало приоритетных сообщений (остатки от коннекта)
  if (mq_receive(ds, new_text, SIZE, &prio) == -1){
    perror("cannot receive");
    return -1;
   }
}
  printf("client: Получили сообщение от сервера: [%s], приоритет = %d\n", new_text, prio);
  printf("client: Отправляем ответку\n");
  
  if (mq_send(ds, text, strlen(text), 4) == -1){
    perror("Sending message error");
    return -1;
  
  if (mq_close(ds) == -1)  // закрываем очередь
    perror("Closing queue error");
  }

 
  return 0;

}
