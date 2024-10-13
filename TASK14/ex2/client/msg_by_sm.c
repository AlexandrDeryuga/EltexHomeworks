#include "msg_by_sm.h"

/* Функция создаст и инициализирует объект очередь, с разделяемой памятью */

myQsys_type *  Qsys_open( char * name)  // обьект очередь, и его имя для дескриптора, версия для системных сообщений
{
int i;
  //создадим обьект, и скопируем его затем в разделяемую память

myQsys_type q,*Q;
Q=&q;  //так, для удобства

// 1.Получаем дескриптор общей памяти 
  if ((Q->shm_fd = shm_open(name, O_CREAT | O_RDWR, 0666)) ==-1){
     perror("cannot open");
     return NULL;
  }
   // 2.Устанавливаем размер общей памяти , это размер массива с сообщениями
  if (ftruncate(Q->shm_fd, sizeof(q)) != 0){
      perror("cannot set size");
      return NULL;
  }
  //3. подключаем память к сегменту
if ((Q->addr = mmap(0,sizeof(q), PROT_WRITE, MAP_SHARED,Q->shm_fd, 0)) == MAP_FAILED){
   perror("cannot mmap");
   return NULL;
  }
  
// остальной конструктор объекта, но там осталось одно поле и массив
Q->mcount=0;
for(i=0;i<SH_SYSMSG_MAX;i++){
   Q->SM_system[i].mType=-1;  // -1 значит свободный
   Q->SM_system[i].priority=Q->mcount;   //подобным образом в будущем будет реализована функция FIFO, сейчас просто 0
   Q->SM_system[i].wait=0;
   strcpy(Q->name,name);
  } 
                                                       
 //4. Блокируем общую память. 
  if (mlock(Q->addr, sizeof(q)) != 0){
      perror("cannot mlock");
      return NULL;
     } 
     
      memcpy(Q->addr, (void*)Q, sizeof(q));    //5 переместим в разделяемую
      
      //6. Разблокировка
    if (munlock(Q->addr, sizeof(q)) != 0){
      perror("cannot mlock");
      return NULL;
     } 
   
   return Q;
}




// Добавить в очередь системных сообщений новое сообщение, для этого смотрим массив со структурой mType=-1; - говорит что свободная
//вернет 0 - успех, -1 ошибка , 

int Qsys_msgsnd( myQsys_type * Q,msgbuf * msg_struct, int wait, float sleeptime )   // аналоги для очереди
{
int i=0;
myQsys_type q;
 
//4. Блокируем общую память. 
  if (mlock(Q->addr, sizeof(q)) != 0){
      perror("cannot mlock");
      return -1;
     } 
 
 
 //блок логики
 if( Q->mcount>=(SH_SYSMSG_MAX-1)) { // некуда добавлять
     if(!wait){
      munlock(Q->addr, sizeof(q)); 
     return -2;  //вернем код того что очередь занята     
     }
   while(Q->mcount>=(SH_SYSMSG_MAX-1)) sleep(sleeptime);
  } 

  while (i<Q->mcount)// найти первый свободный слот в массиве
 {
  if(-1==Q->SM_system[i].mType) break; 
 i++;
 } 
Q->SM_system[i].mType=msg_struct->mType;            //копируем поля
strcpy(Q->SM_system[i].mText,msg_struct->mText);
                                                    //установить системные
 Q->mcount++;    //кол-во сообщений
 Q->SM_system[i].priority=Q->mcount; // для FIFO, при освобождении каждый  не нулевой приоритет уменьшим на 1
                                                    
     
     // memcpy(Q->addr, (void*)SM_system, sizeof(q));    //5 переместим в разделяемую
      
      //6. Разблокировка
      munlock(Q->addr, sizeof(q));   //теперь другие процессы могут лезть в память 
      
      
      //логика nowait
      if(!wait) sleep(sleeptime); // дадим время остальным разгрузить очередь  
      if(wait) while ( ((msgbuf*)Q->addr+i)->mType!=-2 ) { 
      
      sleep(sleeptime); // ждем пока не прочтут, функция чтения сбросит ячейку в -2 
    
                                                  // действия - синхронизация   
        if (mlock(Q->addr, sizeof(q)) != 0){
      perror("cannot mlock");
      return -1;
     } 
      
      //сообщение прочитано, освободим память записав на место последний элемент, и пометим последний как свободный
     Q->SM_system[i].mType=Q->SM_system[Q->mcount-1].mType; 
     Q->SM_system[i].priority=Q->SM_system[Q->mcount-1].priority;
     strcpy(Q->SM_system[i].mText,Q->SM_system[Q->mcount-1].mText);
     Q->SM_system[i].wait=Q->SM_system[Q->mcount-1].wait;
     
     Q->SM_system[Q->mcount-1].mType=-1;
     Q->SM_system[Q->mcount-1].mType=0;   // остальное занулим для облегчения отладки
     Q->SM_system[Q->mcount-1].priority=0;
     Q->SM_system[Q->mcount-1].mText[0]=0;
     Q->SM_system[Q->mcount-1].wait=0;
     
     
     if(Q->mcount>0) {Q->mcount--;} else {  // такого быть не должно !
    };  
     
                                  //и понизим у всех приоритет
                                  
      int ii=0;
      while (ii<Q->mcount) Q->SM_system[ii].priority--;
      
      munlock(Q->addr, sizeof(q));   //теперь другие процессы могут лезть в память 
      return 0;
     }  // end if wait
     
     //если же ждать не надо, помечаем запись
     if (mlock(Q->addr, sizeof(q)) != 0){
      perror("cannot mlock");
      return -1;
     } 
    
    Q->SM_system[i].wait=1;
     
     if (munlock(Q->addr, sizeof(q)) != 0){
      perror("cannot mlock");
      return -1;
     } 
     //////////////////////////////////////
     return 0;  
   }

int sys_msgrcv( myQsys_type * Q, msgbuf *buf, char wait,float sleeptime, int type )
{myQsys_type q;

if(!wait) if(Q->mcount<=0) return -2;// ошибка нечего читать и nowait
while (Q->mcount<=0) sleep (sleeptime); // блокирующий вызов
// Блокируем общую память. 
  if (mlock(Q->addr, sizeof(q)) != 0){
      perror("cannot mlock");
      return -1;
     } 
 // действия    
       int i=0,index=-1;
     //1. найти в массиве очередь с наименьшим приоритетом, для нашего типа#
    
    while (-1==index){
     i=0;
   while (i< Q->mcount){
    if(Q->SM_system[i].mType==type) {
      if(-1==index) index=i;
      if(Q->SM_system[i].priority<Q->SM_system[index].priority) index=i;
     } 
     i++;
   }
   if (munlock(Q->addr, sizeof(q)) != 0){
      perror("cannot mlock");
      return -1;
     } 
     if(!wait) return -2; // ошибка нечего читать и nowait
     sleep(sleeptime);
   }// end while index
   
//прочитать и пометить. 
   
    strcpy(buf->mText,Q->SM_system[index].mText); 
    
    if(1==Q->SM_system[index].wait) {//значит нас ждет send, помечаем
     Q->SM_system[index].mType=-2;
       if (munlock(Q->addr, sizeof(q)) != 0){
      perror("cannot mlock");
      return -1;
     } 
     return 0;
    }
    
  //send не ждет, все сделать самому 
   //сообщение прочитано, освободим память записав на место последний элемент, и пометим последний как свободный
     i=index;
     Q->SM_system[i].mType=Q->SM_system[Q->mcount-1].mType; 
     Q->SM_system[i].priority=Q->SM_system[Q->mcount-1].priority;
     strcpy(Q->SM_system[i].mText,Q->SM_system[Q->mcount-1].mText);
     Q->SM_system[i].wait=Q->SM_system[Q->mcount-1].wait;
     
     Q->SM_system[Q->mcount-1].mType=-1;
     //Q->SM_system[Q->mcount-1].mType=0;   // остальное занулим для облегчения отладки
     Q->SM_system[Q->mcount-1].priority=0;
     Q->SM_system[Q->mcount-1].mText[0]=0;
     Q->SM_system[Q->mcount-1].wait=0;
     
     
     if(Q->mcount>0) {Q->mcount--;} else {  // такого быть не должно !
    };  
    
            //и понизим у всех приоритет
                                  
      int ii=0;
      while (ii<Q->mcount) Q->SM_system[ii].priority--;
  
      
// Разблокировка
      if (munlock(Q->addr, sizeof(q)) != 0){
      perror("cannot mlock");
      return -1;
     } 
    return 1; 
}

void sys_unlink(myQmsg_type *Q)
{
myQmsg_type q;
q.shm_fd=Q->shm_fd;
q.addr=Q->addr;
strcpy(q.name,Q->name);
//7.В конце отделяем сегмент общей памяти от адресного пространства. Это также приведёт к разблокировке этого сегмента, если он блокирован
munmap(q.addr, sizeof(q));
close(q.shm_fd);
//Удаляем сегмент общей памяти
shm_unlink(q.name);
}

void printQ(myQsys_type *Q)
{
printf("Снимок очереди, count=%d\n",Q->mcount);
for (int i=0;i<7;i++) 
 printf("%d# тип=%ld приоритет=%ld wait=%d  сообщение>%s<\n",i,Q->SM_system[i].mType,Q->SM_system[i].priority,Q->SM_system[i].wait,Q->SM_system[i].mText);
}
 
int main ()
{
int i;
msgbuf m;
myQsys_type *Q;
Q=Qsys_open("my_shared") ;
printf("===Qtest====\n");
printQ(Q);
m.mType=1;strcpy(m.mText,"тест 1 тип 1");
Qsys_msgsnd( Q, &m, 0, 0.1);
m.mType=1;strcpy(m.mText,"тест 2 тип 1");
Qsys_msgsnd( Q, &m, 0, 0.1);
m.mType=2;strcpy(m.mText,"тест 3 тип 2");
Qsys_msgsnd( Q, &m, 0, 0.1);
printQ(Q);
}
