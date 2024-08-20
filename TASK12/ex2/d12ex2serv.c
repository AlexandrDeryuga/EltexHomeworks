

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>

#include <sys/stat.h> 
#include <fcntl.h> 

 int main()

{

  int fd_fifo; 

  char buffer[]="Hi!\n";
  char buf[100];

   

  //Если файл с таким именем существует, удалим его

  unlink("fifo0001.1");

  mode_t mode = 0600;
  
  if((mkfifo("fifo0001.1", 0600)) == -1)
  {
    fprintf(stderr, "Невозможно создать fifo\n");
    exit(0);
  }

  
printf("Server FIFO: жду клиента\n");

  if((fd_fifo=open("fifo0001.1", O_WRONLY)) == - 1)   //Открываем fifo для чтения и записи
  {
    fprintf(stderr, "Невозможно открыть fifo\n");
    exit(0);

  }
  printf("Server FIFO: отправляю сообщение клиенту\n");
  write(fd_fifo,buffer,strlen(buffer)) ;

  return 0; 
  }

 
