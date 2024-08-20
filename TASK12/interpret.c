#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include"interpret.h"
                       // непонятно на сколько длинной будет обрабатываемая строка, сделаем динамичекски и так же с аргументами
#define BUFSIZE 1024  // размер буфера для строки интерпритатора , будем перевыделять память при переполнении буфера

#define WBUFSIZE 64                   //буфер -под аргументы- лексемы
#define ARG_DELIM " \t\r\n\a"         // Разделители аргументов : space, tab, carriage return, new line, alert/beep
                                      //для стандартной функции strtok  

//char **words; //на список слов
char *buffer;//буфер командной строки

 
 char *line;    // здесь физически будет расположена строка которую мы введем
  char *conveer; // здесь физически будет расположена строка -конвеер, изначально копия line
  
 char **args; //- массив указателей на аргументы
 char **conv; // массив указателей на элементы конвеера
 char **operands; // массив указателей на операнды, например для оператора перенаправления

 int convFlag;// если 1 то у нас перенеправления данных из процесса в процесс

 int fd_prev_output[2];  //pipe 1
 int fd_next_output[2];  //pipe 2 , для конвеера род номером N будут две очереди, откуда и куда

int main(int argc, char **argv)
{
/*
  char *line;    // здесь физически будет расположена строка которую мы введем
  char *conveer; // здесь физически будет расположена строка -конвеер, изначально копия line
  char **args; //- массив указателей на аргументы
  char **conv; // массив указателей на элементы конвеера
  */
  
  int status,convCount=0;
  
  convFlag=0;

  buffer=NULL; conv=NULL; args=NULL; conv=NULL;operands=NULL;//инициализируем указатели на которых будет выделяться память в будущем 
  
  
  int fd_out; //сохраним сюда копию stdout
  fd_out = dup(fileno(stdout));


  do {
	dup2(fd_out, fileno(stdout)); // вот так должны вернуть назад потом
    printf("interp жду команду> ");
    line = ReadLine();      //прочтем строку
    //printf("interp длина строки>%ld\n",strlen(line));
      conveer = malloc(sizeof(char) * (strlen(line)+1));  // создадим физ. копию 
      if (!buffer) {printf("ошибка выделения памяти\n");ExitFree(EXIT_FAILURE);};
      strcpy(conveer,line);
    if(ValidateInputString(line)) 
    {
    printf("interp обрабатываем>%s\n",conveer);
    
    conv=ParsingLine(conveer, "|");  //получим конвеер    *conv  надо будет освободить
    
    convCount=0;convFlag=0;

    if(conv[0]==NULL) {printf("interp нет конвеера!>"); getchar();}
    
    while (conv[convCount])
    {
    printf("conv>#%s#\n",conv[convCount]);
    if(conv[convCount+1]!=NULL)     //нужно перенаправление данных между процессами
     {
     
      switch (convFlag)  //определим ситуацию с перенаправлением
      {
      case 0:              //это первый
             convFlag=1;  
             break;
      case 1:             //это четный
             convFlag=2; 
             break;
      case 2:             //это нечетный
             convFlag=3; 
             break;
      case 3:             //это снова четный, закольцевали
             convFlag=2; 
             break;
      default:
             printf("Ошибка логики конвеера convFlag=%d\n",convFlag);
             break;
      }
      
      //convFlag=1;
      //dup2(2, 1); // 2-stderr; 1-stdout  00stdin
    } else {  // а здесь по обстоятельствам, так как это последний конвеер
             
             switch (convFlag) 
             { case 0: // перенаправлений не нужно, это был единственный конвеер
                       break;
               case 1: // четный завершающий
                       convFlag=4;
                       //convFlag=3; // заглушка
                       break;
               case 2: // нечетный завершающий
                       convFlag=5;
                       //convFlag=3; // заглушка
                       break;
               case 3: // четный завершающий
                       convFlag=4;
                       //convFlag=3; // заглушка
                       break;
               default:
                       printf("Ошибка логики конвеера - последний, convFlag=%d\n",convFlag);
             }
             
           }
    args = ParsingLine(conv[convCount],ARG_DELIM); //выделим команду и аргументы
    status = Execute(args);
    printf ("interp Execute>команда выполнена с кодом: %d\n", status);

    if(args) {free(args);args=NULL;};
    
    convCount++;


    }
     }
    else printf("iterp> ошибка - строка не прошла валидацию\n");
     
    if(conveer) {free (conveer);conveer=NULL;}
    if(conv)    {free(conv);conv=NULL;}
    if(line)    {free(line);line=NULL;buffer=NULL;}
   
    
  } while (status);
  
  
  
 printf("exit witch clear mem");
  return ExitFree(EXIT_SUCCESS);
}

int ExitFree( int x)      // завершение программы с освобождением памяти
{
/*
char *line;    // здесь физически будет расположена строка которую мы введем
char *conveer; // здесь физически будет расположена строка -конвеер, изначально копия line
char **args; //- массив указателей на аргументы
char **conv; // массив указателей на элементы конвеера
*/

if(line) free(line);
if(conveer) free(conveer);
if(args) free(args);
if(conv) free(conv);
if(operands) free(operands);

exit(x);
}

int ValidateInputString(char *str)   //отлавливаем то что пока не знаем как обрабатывать
{
if(strstr(str,"<<")) return 0;
if(strstr(str,">>")) return 0;

if(strstr(str,"<")&&strstr(str,">")) return 0;
return 1;
}


char *ReadLine(void)       //строку что введет пользователь засунем в буфер перед этим подготовив его
{
  int bufsize = BUFSIZE;
  
  buffer = malloc(sizeof(char) * bufsize);
  if (!buffer) {printf("ошибка выделения памяти\n");ExitFree(EXIT_FAILURE);};
  
  int position = 0;
  int c;

  while (1) 
  {
    c = getchar();  
    if (c == EOF || c == '\n') {buffer[position] = '\0';return buffer;} // заполним буфер до конца строки
                               else {buffer[position] = c;}
    position++;

    
    if (position >= bufsize) {      //Если буфера не хватило - увеличим его
      bufsize += BUFSIZE;
      buffer = realloc(buffer, bufsize);
      if (!buffer) {
                   printf("Ошибка перевыделения памяти\n");
                   ExitFree(EXIT_FAILURE);
                   }
                            }
  }
}



char ** ParsingLine(char *sline, char *div )    // парсинг по элементам: в зависимости от разделителя div это конвеер или команда на запуск
{                                             // с ее аргументами
  int bufsize = WBUFSIZE, counter = 0; //
  char **lexems = malloc(bufsize * sizeof(char*)); //создадим массив указателей на элементы
  char *lexem;

  if (!lexems) { printf("Ошибка выделения памяти\n");ExitFree(EXIT_FAILURE);}

  lexem = strtok(sline, div);
  while (lexem != NULL) 
  { lexems[counter++] = lexem;
   //printf("ParsingLine>%s\n",lexem);
    
    if (counter >= bufsize) 
    {
      bufsize += WBUFSIZE;
      lexems = realloc(lexems, bufsize * sizeof(char*));
      if (!lexems) { printf("Ошибка выделения памяти\n");ExitFree(EXIT_FAILURE);}  
    }

    lexem = strtok(NULL, div); //поиcк следующей лексемы
  
  }
  lexems[counter] = NULL;
  return lexems;
}


int Execute(char **args)                     //Анализ аргумента и спец действия, здесь обрабатываются операторы перенаправления
{
  //char ** operands;  //Массив оперендов
  int count; 
  char argsaved[5000];
  

  
  FILE *file_in=NULL,*file_out=NULL;
  
  int i =0,r=0;
  if (args[0] == NULL) { return 1; }  //ничего не ввели
  if (strcmp(args[0],"exit")==0) ExitFree(0); // покинем интерпетатор
  
                         
  //if(args[0][0]='>')
  //if(args[0][0]='<')
  
  while (args[i])   
  {                             
  ////////////////////////////////////////////////////////////////////////////////////////////
  //      СЕКЦИЯ ОСНОВНОГО АЛГОРИТМА ПЕРЕНАПРАВЛЕНИЙ
  //
  
                                //есть ли в аргументе знак перенаправления
  if (0==strcmp(args[i],">"))  //тогда у нас файл - след аргумент                     -ПЕРЕНАПРАВЛЕНИЕ ВЫХОДА - оттестировано
                             {

                              if(args[i+1]!=NULL)  file_out=freopen (args[i+1],"w",stdout);
                              if(!file_out) {printf ("Execute>ошибка открытия файла\n");return -1;}
                              args[i+1]=NULL;  // оборвем операнды для процесса на этой точке 
                              
                              i=i+2;
                              break;  //и пойдем на обработку следующего аргумента, вдруг там перенаправление другого потока
                              
                             }
                             
  if (0==strcmp(args[i],"<"))  //тогда у нас файл - след аргумент                  - ПЕРЕНАПРАВЛЕНИЕ ВХОДА
                             {
                              if(args[i+1]!=NULL)  file_in=freopen (args[i+1],"r",stdin);
                              if(!file_in) {printf ("Execute>ошибка открытия файла\n");return -1;}
                              args[i+1]=NULL;  // оборвем операнды для процесса на этой точке 
                              
                              i=i+2;
                              break;  //и пойдем на обработку следующего аргумента, вдруг там перенаправление другого потока
                             
                             }
  if(strstr(args[i],">"))      //  перенаправление вывода
     {
      operands=ParsingLine(args[i],">");
      count =0;
      while (operands[count]) count++;
      if(count >2) {printf ("Execute> ошибка - множественное перенаправление");return(-1);}
      // теперь аргумент [i] должен быть обрезан , тогда сделать перенаправление и запустить
      
      printf("Execute operand1>#%s#\n",operands[0]);
      printf("Execute operand2>#%s#\n",operands[1]);
      

   // Замена файла связанного со стандартным потоком вывода stdout.
   // Теперь все данные потока stdout будут записываться в файл, а не
   // выводиться на экран.
   if((operands[0]==NULL)&&(operands[1]!=NULL)) {file_out=freopen (operands[1],"a",stdout); args[i]=NULL;};       // >out
   if((operands[0]!=NULL)&&(operands[1]!=NULL)) {file_out=freopen(operands[1],"a",stdout);  args[i+1]=NULL;};    //...param>out
   if((operands[0]!=NULL)&&(operands[1]==NULL)) {file_out=freopen(operands[0],"a",stdout);    args[i+1]=NULL;}; // param >out                      
      
      if(!file_out) {printf ("Execute>ошибка открытия файла\n"); return -1;}
       break;
       /*r=Launcher(args);
       if(nf) fclose(nf);
       freopen ("/dev/tty", "a", stdout);
       
       if(operands) free(operands);
       operands==NULL;*
     return r; */
     }
     
     
      if(strstr(args[i],"<"))      //  перенаправление входа
     {
      operands=ParsingLine(args[i],"<");
      count =0;
      while (operands[count]) count++;
      if(count >2) {printf ("Execute> ошибка - множественное перенаправление\n");return(-1);}
      // теперь аргумент [i] должен быть обрезан , тогда сделать перенаправление и запустить
      
      printf("Execute operand1>#%s#\n",operands[0]);
      printf("Execute operand2>#%s#\n",operands[1]);
      
                                                                                // если след аргумент это имя файла, то перебросим указатель
   if((operands[0]==NULL)&&(operands[1]!=NULL)) {file_in=freopen (operands[1],"r",stdin);args[i]=NULL;};          // param< in
   if((operands[0]!=NULL)&&(operands[1]!=NULL)) {file_in=freopen(operands[1],"r",stdin);args[i+1]=NULL;};        //...param<in
   if((operands[0]!=NULL)&&(operands[1]==NULL)) {file_in=freopen(operands[0],"r",stdin); args[i]=NULL; };       // param <in                      
      
      if(!file_in) {printf ("Execute>ошибка открытия файла\n");return -1;}
      break;
       
      /* r=Launcher(args);
       if(nf) fclose(nf);
       freopen ("/dev/tty", "r", stdin);
       
       if(operands) {free(operands);operands=NULL;}
       
     return r;*/
     
     }
 
  printf("Execute arg>#%s#\n",args[i]);
  i++;
  
  } //end while
  
  printf("interp> call fork\n");
  r=Launcher(args);   // пойдем запускать через форк
   printf("interp> код возврата %d\n",r);   
     if(file_in)                        // вернем перенаправление назад
          {
          fclose(file_in);
          printf("возврат перенаправления <\n");
           freopen ("/dev/tty", "r", stdin); 
           }
           
        if(file_out)
        {       
         fclose(file_out);
         freopen ("/dev/tty", "w", stdout);
        }
        
       if(operands) {free(operands);operands=NULL;}
       
    //  printf("interp> fork complite \n");
  return r; 
}

int Launcher(char **args)    //ЗАПУСК ЧЕРЕЗ ФОРК
{
  pid_t pid, wpid;
  int status;
  
 char message [5000];
 
//int fd[2];

 int nbytes;
  
  
  int fd_out; //сохраним сюда копию stdout
  fd_out = dup(fileno(stdout));
  int fd_in; //сохраним сюда копию stdin
  fd_in = dup(fileno(stdin)); //сохраним сюда копию stdin
  if((fd_in==-1)||(fd_out==-1)) printf("главная>ошибка создания дескрипторов\n");


	//dup2(fd_out, fileno(stdout)); //так потом вернем назад
	//dup2(fd_in, fileno(stdin)); 

  int i=0;
  while(args[i]) printf("Fork args[%d]>%s\n",i,args[i++]);
  printf("Fork execute>\n");
  
 /* 
  if(convFlag==2)    // это родительский, создаем канал для конструкции типа conveer1 | CONVEER2 | conveer3
    {
      if(pipe(fd_pipe2)!=0) {printf("главная> ошибка очереди\n");}   //создали трубу теперь она должна стать вместо  stdout для ребенка
            else {
                 //здесь пока ничего и не делаем  ЭТО ОСНОВНОЙ ПРОЦЕСС
                  }   
    }
   */ 
    
  if(convFlag==1)    // это родительский, создаем канал    для конструкции типа CONVEER1 | conveer2
    {
	
          if(pipe(fd_prev_output)!=0) {printf("главная> ошибка очереди\n");}   //создали трубу теперь она должна стать вместо  stdout для ребенка
            else { //здесь пока ничего и не делаем  ЭТО ОСНОВНОЙ ПРОЦЕСС
                 }
          if(pipe(fd_next_output)!=0) {printf("главная> ошибка очереди\n");}  //и еще одну на случай нескольких конвееров
            else {    }
                  
    }

  pid = fork();
  if (pid == 0) 
{  //это дочерний процесс

  /* 
   if(convFlag==1) {close(fd_prev_output[0]);          // это не сработало        
                     close(fd_next_output[0]);};
*/
/*	   
	   if(convFlag==3) 
	    {//close(fd_prev_output[1]);
	     printf("перенаправление конвеера3>\n");
	     //close(0);
	     dup2(fd_prev_output[0],fileno(stdin));//   stdin делаем копией fd[0];  т.е весь ввод дочернего процесса в канал
	    }
	 
	  if(convFlag==1)
	      {   
	          //close(fd_prev_output[0]);
		  printf("перенаправление конвеера1>\n");
		  //close(1);
		  //dup(fd[0]);//dup использует самый маленький номер, поэтому это 1 ( close(1) )
		  
	  	  dup2(fd_prev_output[1],fileno(stdout));//   stdout делаем копией fd[1];  т.е весь вывод дочернего процесса в канал
	  	// convFlag=2; //перенаправление совершено, нужен будет возврат

	  	 
	      }
*/	      

switch (convFlag){
case 1://CONVEER1|conveer2|...
       dup2(fd_prev_output[1],fileno(stdout)); //   stdout делаем копией fd[1];  т.е весь вывод дочернего процесса в канал prev
       break;

case 2:  //  ..|CONVEER 2|conveer 3|..          // Для четного конвеера [0] -чтение [1] запись
       dup2(fd_prev_output[0],fileno(stdin));  //   stdin делаем копией fd[0];   т.е весь ввод дочернего процесса из канала  prev
       dup2(fd_next_output[1],fileno(stdout)); //   stdout делаем копией fd[1];  т.е весь вывод дочернего процесса в канал   next   
       break;
case 3: dup2(fd_next_output[0],fileno(stdin));  // Для нечетного конвеера наоборот  теперь next это поток входных данных
        dup2(fd_prev_output[1],fileno(stdout));
       break;
case 4: //для последнего конвеера, если он четный
       printf("4:\n");
       dup2(fd_prev_output[0],fileno(stdin));//   stdin делаем копией fd[0];  т.е весь ввод дочернего процесса в канал
      
       dup2(fd_out, fileno(stdout)); // вернем назад вывод на экран
       
       close(fd_prev_output[1]);
       break;   
case 5: //для последнего конвеера, если он нечетный
       printf("5:\n");
       //close(0);close(1);
       dup2(fd_next_output[0],fileno(stdin));//   stdin делаем копией fd[0];  т.е весь ввод дочернего процесса в канал
       close(fd_next_output[1]); // нужно [1]
       close(fd_prev_output[1]);  // не зависит
       
       dup2(fd_out, fileno(stdout)); // вернем назад вывод на экран
      
       //dup2(fd_out, fileno(stdout)); // вот так должны вернуть назад потом
       
       //close (0);
       break;      
}
	      
  if (execvp(args[0], args) == -1) {perror("Run exec error");} // подменяем код на код команды args[0] и передаем ей аргументы вызова
  ExitFree(EXIT_FAILURE);
 } else if (pid < 0) { perror("fork eroror");    // Error forking
                        printf("ошибка форк\n");
                         } 
       else 
  {
    // код родителя - будем ждать завершение нашего процесса, но не в мертвую а работая
    //switch (convFlag)
    
 
      if(convFlag==5) close(fd_next_output[1]); //хотим получать данные от ребенка, значит [1] отправить значит 
      if(convFlag==1) {
                         close(fd_prev_output[1]);
                         //close(fd_next_output[1]); //нельзя закрывать сразу
                       }
      
    //if(convFlag==5) close(fd_next_output[1]); //хотим получать данные от ребенка, значит [1] отправить значит 
    //if(convFlag==3) close(fd_prev_output[0]);
    do {
      wpid = waitpid(pid, &status, WUNTRACED);
    } while (!WIFEXITED(status) && !WIFSIGNALED(status));
    printf ("interp FORK>команда выполнена\n");
    //по идее можно не перенаправлять
    /*
    switch(convFlag)
    {
     case 5:
     case 4:
     //close(0);
     //close(1);
     printf("закрытия перенаправлений\n");
     close(fd_prev_output[0]);
     close(fd_next_output[0]);
     close(fd_prev_output[1]);
     close(fd_next_output[1]);
     break;
     default:
             break;
    }
   */
  }
  //if(convFlag==1)convFlag=3;
  return 1;
}
