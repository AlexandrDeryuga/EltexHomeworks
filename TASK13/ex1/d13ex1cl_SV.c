#include <sys/msg.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

struct msgbuf {
   long mType;
   char mText[50];
};

int main() {
   char txtBuf[50];
   int qId_Recive,qId_Send;                     //идентификаторы очередей
   key_t key_Send, key_Recive;                  // уникальные ключи для очередей
  
  struct msgbuf msg, buf;                      // одно сообщение на прием и отправку
   struct msqid_ds msgCtlBuf; 
                                                    
                                                      // генерируем ключи очередей
   if ( ( key_Send = ftok( "/tmp", 1 ) ) == -1 ) {   // сюда пишем серверу
      perror( "client: ftok failed:" );
      exit( 1 );
   }
   
     if ( ( key_Recive = ftok( "/tmp", 2 ) ) == -1 ) {// отсюда читаем что шлет сервер
      perror( "client: ftok failed:" );
      exit( 1 );
   }

   printf( "client: System V IPC\nkey_Recive=%u  key_Send = %u\n",key_Recive , key_Send );
   
                                                           //создаем очереди
   if ( ( qId_Send = msgget( key_Send, IPC_CREAT | 0666 ) ) == -1 ) {  
      perror( "client: Failed to create message queue:" );
      exit( 2 );
   }
     if ( ( qId_Recive = msgget( key_Recive, IPC_CREAT | 0666 ) ) == -1 ) {  
      perror( "client: Failed to create message queue:" );
      exit( 2 );
   }

   printf( "client: id очередей сообщений  qId_Recive=%u qID_Send= %u \n", qId_Recive,qId_Send );
   
                                                                      // Прочитать сообщение от сервера
   if ( msgrcv( qId_Recive, &buf, sizeof msg.mText, 1, 0 ) == -1 )   //
      perror( "client: msgrcv failed:" );
   else
      printf( "client: Получено сообщение от сервера = %s\n", buf.mText );
      

   strcpy( msg.mText, "Hello! Я клиент." );         // отправить ответку серверу
    msg.mType = 1;
   
   if ( msgsnd( qId_Send, &msg, sizeof msg.mText, 0 ) == -1 ) {
      perror( "cliebt: msgsnd failed:" );
      exit( 3 );
   }
       printf( "client: Ответное сообщение отправлено\n" );
       printf( "client: тест - Жду следующее сообщение от сервера - его не будет, и я уйду в ошибку\n" );
  
   //  Посмотрим на ошибки - сервер удалит очередь пока я жду
   if ( msgrcv( qId_Recive, &buf, sizeof msg.mText, 1, 0 ) == -1 )
      perror( "client: msgrcv failed:" );
   else
      printf( "client: Message received = %s\n", buf.mText );

   printf( "client: errno = %d\n", errno );
   // printf( "client: вот такие вот ошибки, завершаю работу...\n" );

  if ( msgrcv( qId_Recive, &buf, sizeof msg.mText, 1, 0 ) == -1 )
      perror( "client: msgrcv failed:" );
   else
      printf( "client: Message received = %s\n", buf.mText );

   printf( "client: errno = %d\n", errno );
    printf( "client: вот такие вот ошибки, завершаю работу...\n" );
}



