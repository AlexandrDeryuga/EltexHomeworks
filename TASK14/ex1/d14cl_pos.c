#include <unistd.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>

#include <semaphore.h>

#define SHARED_MEMORY_OBJECT_NAME "my_shared_memory"
#define SHARED_MEMORY_OBJECT_SIZE 50
#define MSG_TYPE_EMPTY  0 // пустое сообщение 
#define MSG_TYPE_STRING 1 // тип сообщения о том, что передана непустая строка
#define MSG_TYPE_FINISH 2 // тип сообщения о том, что  пора завершать обмен 

typedef struct myshmem_type{
int type; //укажем тип , аналог систем 5
char str[SHARED_MEMORY_OBJECT_SIZE];
} myshmem_type;

int main()
{
char str[SHARED_MEMORY_OBJECT_SIZE];
int shm_fd; // дескриптор
void *vaddr; //адрес обьекта
int len;
myshmem_type myshmem;

printf("Client запущен и ждет сервера..");

 // 1.Получаем дескриптор общей памяти 

  if ((shm_fd = shm_open(SHARED_MEMORY_OBJECT_NAME, O_CREAT | O_RDWR, 0666)) ==-1){
     perror("cannot open");
     return -1;
  }


  // 2.Устанавливаем размер общей памяти 
  if (ftruncate(shm_fd, sizeof(myshmem_type)) != 0){
      perror("cannot set size");
      return -1;
  }

//3. подключаем память к сегменту
if ((vaddr = mmap(0,sizeof(myshmem_type), PROT_WRITE, MAP_SHARED,shm_fd, 0)) == MAP_FAILED){
   perror("cannot mmap");
   return -1;
  }

 //4. Блокируем общую память. 

  if (mlock(vaddr, sizeof(myshmem_type)) != 0){
      perror("cannot mlock");
      return -1;
}

// 5. Действия
printf("Client память=%s\n",((myshmem_type *)vaddr)->str);

myshmem.type=MSG_TYPE_FINISH;
strcpy(myshmem.str,"hi! i'm client string!");
memcpy(vaddr, (void*)&myshmem, sizeof(myshmem));




//6. Разблокировка
 munlock(vaddr, sizeof(myshmem_type));


//7.В конце отделяем сегмент общей памяти от адресного пространства. Это также приведёт к разблокировке этого сегмента, если он блокирован

printf("Client выхожу..\n");
munmap(vaddr, sizeof(myshmem_type));
close(shm_fd);

//Удаляем сегмент общей памяти - это дело сервера
//shm_unlink("my_shm");
}

