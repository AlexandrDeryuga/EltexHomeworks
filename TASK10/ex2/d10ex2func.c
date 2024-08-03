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
     
     
     typedef struct list 
{
int Number;      //для удобства порядковый номер узла
int FirstChild;
//int TreeLevel;  //для удобства уровень дерева
char Name[20];
int CountChilds; //кол-во дочерних узлов
int TimeSleep;
} list;

int ListCounter=0;  //указатель на узел обработки
list Tree[10];

void InitList( int i,int firstchild, char *name, int childs, int timesleep)
{
strcpy(Tree[ListCounter].Name,name);
Tree[ListCounter].CountChilds=childs;
Tree[ListCounter].TimeSleep=timesleep;
Tree[ListCounter].Number=i;
Tree[ListCounter].FirstChild=firstchild;
//Tree[ListCounter].TreeLevel=level;

ListCounter++;
Tree[ListCounter].Number=-1; // указатель конца дерева
}


void main(int argc, char *argv[])
{ 

InitList(0,1,"Дуб",2,0);
InitList(1,3,"Ясень",2,0);
InitList(2,5,"Елка",1,0);
InitList(3,-1,"Клен",0,5);
InitList(4,-1,"Тополь",0,10);
InitList(5,-1,"Сосна",0,0);

char ZAPAH[50][50];

strcpy (ZAPAH[0],"дуба"); //0
strcpy (ZAPAH[1],"ясеня"); //1
strcpy (ZAPAH[2],"елки"); //2
strcpy (ZAPAH[3],"клена"); //3
strcpy (ZAPAH[4],"тополя"); //4
strcpy (ZAPAH[5],"сосны"); //5
strcpy (ZAPAH[6],"не рожденного дерева"); //6

int MAX=6;
 char str[1000];

int status; 
long ListSeek=0;  //будем использовать atol - что бы не зависить от архитектуры   - это наш указатель на элемент дерева в массиве

 pid_t pid,pid2;
 

int i=0;

if (argc>0) {ListSeek=atol(argv[0]);} else  ListSeek=0;

if(Tree[ListSeek].Number==-1) exit(-1); //программа - дерево закончена
if(!Tree[ListSeek].CountChilds){ //некого порождать, спим по программе и на выход

                                printf("[%d] %s сплю %d секунд и выхожу c кодом %d\n",getpid(),Tree[ListSeek].Name,Tree[ListSeek].TimeSleep,Tree[ListSeek].Number);
                                sleep(Tree[ListSeek].TimeSleep);
                                exit(Tree[ListSeek].Number);

                               }

 printf("\n#[%d] головной, мой предок [%d]: Я %s\n",getpid(),getppid(),Tree[ListSeek].Name);
 //printf("\n#[%d] головной, мой предок [%d]: Я %s  ListSeek=%d, argc=%d\n",getpid(),getppid(),Tree[ListSeek].Name,(int) ListSeek,argc);
 //getchar();
i=0;
int ListNumber=Tree[ListSeek].FirstChild;
while(1)
          { if(i>=Tree[ListSeek].CountChilds) break;
                 if(pid=fork()) {//головной 
                 
    //printf("[%d] Я %s, мой предок [%d]: i=%d, ListSeek=%d, ждать %d детей\n",getpid(), Tree[ListSeek].Name,getppid(),i,(int) ListSeek, Tree[ListSeek].CountChilds);
                                  
                                 }
                                 else
                                 {// я ребенок
                                
                               //printf("[%d] я %s, мой родитель %d, Number=%d, ListSeek=%ld , i=%d\n",pid,Tree[ListNumber].Name,getppid(),ListNumber,ListSeek,i);
                                 printf("[%d] я %s, мой родитель %d\n",pid,Tree[ListNumber].Name,getppid());
                             
                                 sprintf(str,"%d",ListNumber);
                               //printf("[%d] аргумент на запуск %s\n",getpid(),str);
                                
                                 execl("./d10ex2f",str,NULL);
                                 printf("execl не вызвался");
                                 exit(-1);
                                 }
              i++; ListNumber++;
              }
              for(int j=0;j<Tree[ListSeek].CountChilds;j++) 
                                   {wait(&status); 
                                    printf("[%d] %s слышу запах %s\n",getpid(),Tree[ListSeek].Name,ZAPAH[WEXITSTATUS(status)]);
                                   }
                                   printf("[%d] %s все мои отростки расцвели, зацветаю c кодом %d\n",getpid(),Tree[ListSeek].Name,Tree[ListSeek].Number);
                                   exit(Tree[ListSeek].Number);

}

