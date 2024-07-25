#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

 

/*
Процесс после системного вызова fork, раздваивается, у исходного процесса создаётся идентичный потомок-двойник в идентичном состоянии (ну почти). Создавшийся процесс будет занят выполнением того же кода ровно с той же точки, что и исходный процесс.

Различить кто создал, а кто создался, можно по возвращаемому значению fork, поэтому его результат обычно передаётся в if, чтобы эти процессы выполнили какие-то разные вещи, один пошёл в ветку if, другой в ветку else.
*/

 

#include <fcntl.h>                                         
     

void main()
{ 

char ZAPAH[50][50];


strcpy (ZAPAH[0],"дуба"); //0
strcpy (ZAPAH[1],"сосны"); //1
strcpy (ZAPAH[2],"елки"); //2
strcpy (ZAPAH[3],"клена"); //3
strcpy (ZAPAH[4],"тополя"); //4
strcpy (ZAPAH[5],"ясеня"); //5
strcpy (ZAPAH[6],"не рожденного дерева"); //6



int status;
 pid_t pid1,pid2,pidSosna,pidElka,pidYasen,pidKlen,pidTopol;

pidYasen=fork();  //вилка1
if (-1==pidYasen)  {printf("Ясень не родился - ошибка\n");
              perror("fork"); // произошла ошибка 
               exit(6); }
             
if(!pidYasen){  // я первый потомок
             
            pidKlen=fork(); 
            if (-1==pidKlen)  {printf("Клен не родился - ошибка\n");
                                perror("fork"); // произошла ошибка 
                                exit(6); 
                              }
            if(!pidKlen) {// я клен, сын ясеня
                               printf("3)я Клен[%d], сын Ясеня[%d] - жру 5 секунд и расцветаю\n" , getpid(),getppid());
                               sleep(5);
                               exit(3);
                         }
                        else {
                        pidTopol=fork();
                         if (-1==pidTopol)  {printf("Тополь не родился - ошибка\n");
                                            perror("fork"); // произошла ошибка 
                                            exit(6); }
                        if(!pidTopol)
                        {//я тополь
                         printf("4)я Тополь[%d], сын Ясеня[%d] - жру 10 секунд и расцветаю\n" , getpid(),getppid());
                               sleep(10);
                               exit(4);
                        } else
                           {
                           // я ясень
                            printf("5)я Ясень[%d], сын Дуба[%d] - жду пока зацветут мои сыновья Клен[%d] и Тополь[%d]\n" , getpid(),getppid(),(int) pidKlen,(int)pidTopol);
                               wait(&status);
                              printf("ЯСЕНЬ: Слышу, запах %s\n", ZAPAH[WEXITSTATUS(status)]); 
                               wait(&status);
                              printf("ЯСЕНЬ: Слышу, запах %s\n", ZAPAH[WEXITSTATUS(status)]);  
                            
                               exit(5);
                           }
                        
                        
                              }
             
         }
         else
    {
    //Я головной процесс дерева
    
    pidElka=fork();  //вилка2
    if (-1==pidElka)  {printf("елка не родилась - ошибка\n");
                   perror("fork"); // произошла ошибка 
                   exit(6); }
      if(!pidElka){  // я елка, дочь дуба
                pidSosna=fork();
                if (-1==pidSosna)  {printf("Сосна не родилася - ошибка\n");
                                            perror("fork"); // произошла ошибка 
                                            exit(1); }
                if(!pidSosna){ //Я сосна - расцветаю
                              printf("1)я Сосна[%d], дочь Елки[%d] - расцветаю немедленно\n" , getpid(),getppid());
                              exit(1);
                             }
                             else{ //Я елка
                                   printf("2)я Елка[%d], дочь Дуба[%d] - жду Сосну[%d]\n" , getpid(),getppid(),(int)pidSosna);
                                  wait(&status);
                                  printf("ЕЛКА: Слышу запах %s, и я расцветаю\n",ZAPAH[WEXITSTATUS(status)]);
                                  exit(2);
                                 }
                                            
              }
         else
    {
    //Я головной процесс дерева
    printf("0) Я могучий дуб[%d] от рода[%d], родил Ясень [%d] и Елку[%d], жду пока рацветут..\n",getpid(),getppid(),pidYasen,pidElka);
         wait(&status);
         printf("ДУБ: Слышу, запах %s\n", ZAPAH[WEXITSTATUS(status)]); 
          wait(&status);
         printf("ДУБ: Слышу, запах %s\n", ZAPAH[WEXITSTATUS(status)]);  
         
          printf("ДУБ: все мои потомки расцвели! Миссия выполнена!");
          exit (0);
    }
             
}
}


