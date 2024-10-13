#include <unistd.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>

#include <semaphore.h>
#include <sys/stat.h>
#include <errno.h>

#define SYSBUF 250
#define BIGBUF 1000
#define MAX_ROOMS 100  // максимальное кол-во клиентов

#define SH_SYSMSG_MAX 200  // кол-во сообщений в очередях
#define SH_BIGMSG_MAX 16 


/* строим очереди сообщений на разделяемой памяти*/


typedef struct msgbuf {
   long mType;
   long priority;         
   char mText[SYSBUF];
   char wait;
}msgbuf;

typedef struct msgBigBuf {
   long mType;
   long priority;
   char mText[BIGBUF];
   char wait;
}msgBigBuf;

typedef struct myQsys_type
{
int mcount;
int shm_fd;//id для разделяемой памяти
void *addr; // адрес разделяемой памяти
char  name[256];
msgbuf SM_system[SH_SYSMSG_MAX];
} myQsys_type;

typedef struct myQmsg_type
{
int mcount;
int shm_fd;//id для разделяемой памяти
void *addr; // адрес разделяемой памяти
char  name[256];
msgBigBuf SM_system [SH_SYSMSG_MAX];
} myQmsg_type;


myQsys_type *  Qsys_open( char * name, int init);  // обьект очередь, и его имя для дескриптора, версия для системных сообщений
myQmsg_type *  Q_open( char * name, int init);  // обьект очередь, и его имя для дескриптора, версия для системных сообщений

int Qsys_msgrcv( myQsys_type * Q, msgbuf *buf, char wait,float sleeptime, int type );
int Qsys_msgsnd( myQsys_type * Q,msgbuf * msg_struct, int wait, float sleeptime );

int Q_msgrcv( myQmsg_type * Q, msgBigBuf *buf, char wait,float sleeptime, int type );
int Q_msgsnd( myQmsg_type * Q, msgBigBuf * msg_struct, int wait, float sleeptime );

void sys_unlink(myQsys_type *Q);
void msg_unlink(myQmsg_type *Q);

void printQ(myQsys_type *Q);
