

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
  char buf[100];


printf("Client FIFO> жду сервера\n");    //Открываем fifo для чтения 

  if((fd_fifo=open("fifo0001.1", O_RDONLY)) == - 1)
         {
          fprintf(stderr, "Невозможно открыть fifo\n");
          exit(0);  
         }

  if(read(fd_fifo, &buf, sizeof(buf)) == -1) fprintf(stderr, "Невозможно прочесть из FIFO\n");
   else  printf("Client FIFO> получено сообщение от сервера: %s\n",buf);
  
  unlink("fifo0001.1");  

  return 0; 
  }
