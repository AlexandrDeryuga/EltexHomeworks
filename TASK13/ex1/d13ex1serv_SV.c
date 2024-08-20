#include <sys/msg.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct msgbuf {
   long mType;
   char mText[50];
};

int main() {
   char txtBuf[50];
   int qId_Recive,qId_Send;
   key_t key_Recive,key_Send; //создал 2 очереди, вообще можно было обойтись и одной, через  mType в структуре msgbuf
   struct msgbuf msg, buf;
   struct msqid_ds msgCtlBuf;

                                                     // генерируем ключи очередей
   if ( ( key_Send = ftok( "/tmp", 2 ) ) == -1 ) {   // сюда пишем клиенту
      perror( "client: ftok failed:" );
      exit( 1 );
   }
   
     if ( ( key_Recive = ftok( "/tmp", 1 ) ) == -1 ) {// отсюда читаем что шлет клиент
      perror( "client: ftok failed:" );
      exit( 1 );
   }

   printf( "server: System V IPC\nkey_Send = %u  key_Recive=%u \n", key_Send, key_Recive );


  
   if ( ( qId_Send = msgget( key_Send, IPC_CREAT | 0666 ) ) == -1 ) {   //создаем очередь сообщений 
      perror( "server: Failed to create message queue:" );
      exit( 2 );
   }

   if ( ( qId_Recive = msgget( key_Recive, IPC_CREAT | 0666 ) ) == -1 ) {   
      perror( "server: Failed to create message queue:" );
      exit( 2 );
   }
   
   printf( "server:  id очередей сообщений  Send = %u   Recive=%u\n", qId_Send, qId_Recive );

   strcpy( msg.mText, "Hi! Я-сервер." );
   msg.mType = 1;

   if ( msgsnd( qId_Send, &msg, sizeof msg.mText, 0 ) == -1 ) {
      perror( "server: msgsnd failed:" );
      exit( 3 );
   }

   printf( "server: Сообщение отправлено, жду ответа..\n" );
   
      // Прочитать сообщение
   if ( msgrcv( qId_Recive, &buf, sizeof msg.mText, 1, 0 ) == -1 )
      perror( "server: msgrcv failed:" );
   else
      printf( "server: Получено сообщение от клиента = %s\n", buf.mText );

   printf( "server: тест - жду 10 секунд и удалю очередь\n" );
   sleep( 10 );
   

   if ( msgctl( qId_Recive, IPC_RMID, &msgCtlBuf ) == -1 ) {             //удаляем очередь
      perror( "server: msgctl failed:" );
      exit( 4 );
   }
   
    if ( msgctl( qId_Send, IPC_RMID, &msgCtlBuf ) == -1 ) {             //удаляем очередь
      perror( "server: msgctl failed:" );
      exit( 4 );
   }
 printf( "server: очереди удалены, завершаю работу...\n" );
}
