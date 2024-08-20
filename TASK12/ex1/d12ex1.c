#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>

int main(void)
{
        int     fd[2];
        int nbytes;//сколько данных было считанно через read
        pid_t   childpid;
        char    string[] = "Hi!\n";
        char    message[100];

         if(pipe(fd) < 0){// Если создать pipe не удалось, обработчик
                           printf("Can\'t create pipe\n");
                           exit(-1); 
                         } 
        
        if((childpid = fork()) == -1)
        {
                perror("fork");
                exit(1);
        }

        if(childpid == 0)
        {
                // это ребенок
                close(fd[0]);// мы передаем, поэтому закрываем ненужное

                write(fd[1], string, (strlen(string)+1));// запись строки в трубку
                close(fd[1]);
                exit(0);
        }
        else
        {
                //  код родителя
                close(fd[1]); // нужно получить данные, поэтому закроем ненужное

                nbytes = read(fd[0], message, sizeof(message));  //прочитаем трубу
                printf("Received string: %s", message);
        }
        close(fd[0]);
        return(0);
}



