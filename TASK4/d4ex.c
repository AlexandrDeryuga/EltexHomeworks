#include <stdio.h>

#define MAX 3    //кол-во записей в абонентской книге
#define STR_LENGTH 10

struct abonent {
 char name[STR_LENGTH];
 char second_name[STR_LENGTH];
 char tel[STR_LENGTH];
 } ABOOK[MAX+1];   //создадим уж наш справочник, последняя запись используется для фильтра 
 int count=0; // кол-во записей 

void PrintMenu();            //выводит меню на экран
unsigned char GetCommand(); // Определяет какой из пунктов меню выбрал пользователь
void PrintItem(int item );  //печатает элемент структуры по индексу 0-MAX
unsigned char AddItem();    //Добавит нового абонента в справочник
void FindAbonent();          //Фильтрация абонентов
int FillStructure(int Index, int param); //сканируем и заполним структуру по индексу введенными данными
char * FindSTR(const char * str1,const char *str2);  //функция поиска подстроки из предыдущего задания
void DeleteItem(); // удалить абонента из справочника

enum MENU {BOOK_Add=1, BOOK_Delete,BOOK_Find, BOOK_PrintAll, BOOK_Exit} mymenu;


void main ()
{

int i=0,j=0,z=0;

//z=getchar(); printf("%d",z);

for(i=0;i<=MAX;i++) for(j=0;j<STR_LENGTH;j++) //Инициализация справочника
  {
   ABOOK[i].name[j]=0;
   ABOOK[i].second_name[j]=0;
   ABOOK[i].tel[j]=0;
  }
       
    
while(mymenu!=BOOK_Exit)
 {
 PrintMenu();
 while (!GetCommand()) {};//процедура установит mymenu
 
 switch (mymenu)
 {case BOOK_Add:    AddItem();break;
  case BOOK_Delete: DeleteItem();break;
  case BOOK_Find:   FindAbonent();break;
  case BOOK_PrintAll: 
                    if(count==0) {printf("Нет записей в справочнике\n");break;};
                    printf("Список абонентов (%d записей):\n",count);
                    for(i=0;i<count;i++) PrintItem(i);                  
                    break; 
 };
 i++;
 }
}

void PrintItem(int item)
{if ((item>=0)&&(item<=MAX))
 {int i=0; 
 printf("#%2d Имя: ",item+1);
 for(i=0;i<STR_LENGTH;i++) if(ABOOK[item].name[i]){printf("%c",ABOOK[item].name[i]);}else{printf(" ");};
printf(" Фамилия: ");
for(i=0;i<STR_LENGTH;i++) if(ABOOK[item].second_name[i]){printf("%c",ABOOK[item].second_name[i]);}else{printf(" ");};
printf(" Телефон: ");
for(i=0;i<STR_LENGTH;i++) if(ABOOK[item].tel[i]){printf("%c",ABOOK[item].tel[i]);}else{printf(" ");};
printf("\n");
 } else {printf("index error\n");};
  
}

int  FillStructure(int Index,int param)// param=0 - строгий ввод аргументов
{int flag=3;
 int i=0; 
 int ch=0,L=0;
ch=getchar();
L=STR_LENGTH;
while (ch=='\n') {ch=getchar();}                            // scanf(%10s %10s %10s ) через гетчар
while (ch!='\n') {
                  if((param)&&(ch=='*')&&(i==0)) break; // если не строго, и * - то пропускаем
                  if(i>=L) {//while ((ch!='\n')||(ch!=' ')) {ch=getchar();}; //если выход за границу поля, то отбрасываем остаток
                            break;};
                  if(ch==' ') break;
                  if((ch>=48)&&(ch<=122)) // пропустим в обработку только цифры и английские литеры
                  {
                  ABOOK[Index].name[i]=ch;
                  i++;
                  } else {flag=-1;}
                  ch=getchar();
                  } 
if(ch=='\n') return 1;
i=0;ch=getchar();
while (ch==' ') {ch=getchar();}
while (ch!='\n') {
                  if((param)&&(ch=='*')&&(i==0)) break; // если не строго, и * - то пропускаем
                  if(i>=L) {//while ((ch!='\n')||(ch!=' ')) {ch=getchar();}; //если выход за границу поля, то отбрасываем остаток
                            break;};
                  if(ch==' ') break;
                  if((ch>=48)&&(ch<=122)) 
                  {
                  ABOOK[Index].second_name[i]=ch;
                  i++;
                  } else {flag=-1;}
                  ch=getchar();
                  }
if(ch=='\n') return 2;
i=0;ch=getchar();
while (ch==' ') {ch=getchar();}
while (ch!='\n') {if((param)&&(ch=='*')&&(i==0)) break; // если не строго, и * - то пропускаем
                  if(i>=L) {//while ((ch!='\n')||(ch!=' ')) {ch=getchar();}; //если выход за границу поля, то отбрасываем остаток
                            break;};
                  if((ch>=48)&&(ch<=122)) 
                  {
                  ABOOK[Index].tel[i]=ch;
                  i++;
                  } else {flag=-3;
                          }
                  ch=getchar();
                  }      //по итогу должны иметь аккуратно заполненную структуру либо флаг неверной раскладки

if((param)&&(0==ABOOK[Index].tel[0])&&(0==ABOOK[Index].name[0])&&(0==ABOOK[Index].second_name[0])) return 0; // структура не заполнена, для поиска не введены параметры
return flag;// 
}

unsigned char AddItem()
{
int ch;char flag=1;
int i=0;
if(count>=MAX) {printf("Справочник переполнен, удалите ненужных абонентов\n");return 0;};

while (flag)
{flag=0;
printf("Введите информацию об абоненте (имя фамилия телефон):\n");
printf("Используйте только английские буквы(что бы уложиться в байт) %d символов на поле:\n",STR_LENGTH);
//scanf("%10s %10s %10s",ABOOK[count].name,ABOOK[count].second_name,ABOOK[count].tel);

i=FillStructure(count,0); //0- все аргументы следует заполнить
//printf ("код возврата =%d\n",i);
if(i!=3) {flag=1;
           for(i=0;i<STR_LENGTH;i++) 
            {ABOOK[count].name[i]=0;
             ABOOK[count].second_name[i]=0;
             ABOOK[count].tel[i]=0;
             }
         } 
}


 printf("Добавлена запись:\n"); 
 count++;     // увеличиваем счетчик. но печать то по индексу идет
 PrintItem(count-1);
  printf("Запись корректна? (y/n):\n");
  scanf("%*[^YyNn]");
  ch = getchar();
  if ((ch=='N')||(ch=='n')) //Откат
   {
    count--;
    for(i=0;i<STR_LENGTH;i++) 
       {ABOOK[count].name[i]=0;
        ABOOK[count].second_name[i]=0;
        ABOOK[count].tel[i]=0;
      }
    }
}

void FindAbonent()
{int i=0,findcount=0;
char A[MAX]; //Массив указывающий подходит ли  из списка 

for(i=0;i<MAX;i++) A[i]=0;
 
printf("Введите имя фамилию или номер телефона абонента для поиска:\n");
printf("формат(Английская раскладка): Имя/* Фамилия/* телефон/* \n");
i=0;
while(i!=3)
 {
 i=FillStructure(MAX,1); // Заполним строки, по которым будем искать, параметр =1 доспускает * (не заполнение какого либо поля структуры) 
 if(i==0) printf("не задан фильтр\n"); // введено было 3 звездочки * * *   
 if(i==-1)printf("не верный формат\n");
 if(i!=3) printf("повторите ввод:\n");
 }
 
  printf("ищем совпадения по фильтру:\n");
  PrintItem(MAX);
  printf("================================================\n");
  //printf("код возврата: %d\n",i);

  char Z[1000];char *x=NULL;
  if(0!=ABOOK[MAX].name[0]) //найдем здесь записи подходящие для нашего фильтра
     {for(i=0;i<MAX;i++) {x=FindSTR(ABOOK[i].name,ABOOK[MAX].name);
                         if(x!=NULL){ A[i]=1;printf("%s",x);}
                         x=NULL;
                         }
      }
  if(0!=ABOOK[MAX].second_name[0]) //найдем здесь записи
     {for(i=0;i<MAX;i++){ x=FindSTR(ABOOK[i].second_name,ABOOK[MAX].second_name);
                        if(NULL!=x) {A[i]=1;printf("%s ",x);}
                        x=NULL;
                        }
    }
   if(0!=ABOOK[MAX].tel[0]) //найдем здесь записи
     {for(i=0;i<MAX;i++) {x=FindSTR(ABOOK[i].tel,ABOOK[MAX].tel);
                          if(NULL!=x) {A[i]=1;printf("%s ",x);}
                          x=NULL;
                        }
     }
     
  for(i=0;i<MAX;i++) if(A[i]) findcount++;
  printf("найдено %d записей по фильтру:\n", findcount);
  printf("================================================\n");
  for(i=0;i<MAX;i++) if(A[i]) PrintItem(i);
  
    
    for(i=0;i<STR_LENGTH;i++) 
       {ABOOK[MAX].name[i]=0;
        ABOOK[MAX].second_name[i]=0;
        ABOOK[MAX].tel[i]=0;
      }

}

unsigned char GetCommand()
{
unsigned char ch=0; 
printf("Ваш выбор (1-5):\n");
ch=getchar();
while (ch=='\n') {ch=getchar();}

while (getchar()!= '\n') {ch=0;};
//getchar();
if((ch>=49)&&(ch<=53)) {mymenu=ch-48;return(mymenu);};
return 0;
}

void DeleteItem()
{
int z,i;
if(0==count) {printf("Справочник пуст! нет записей на удаление\n");return;}

printf("Для удаления найдите абонента, пример: * Savina *\n");
FindAbonent();
printf("Введите номер записи для удаления (от 1 до %d):\n",count);
scanf("%d",&z);
z--;  //отображение перевели к индексу

if((z>=0)&&(z<count))  //Будем удалять
  {
    printf("Будет удалена запись\n");
    PrintItem(z);
    printf("Удалить (y/n):\n");
    scanf("%*[^YyNn]");
    int ch = getchar();
    if ((ch=='N')||(ch=='n')) return;
    
    for(i=0;i<STR_LENGTH;i++)                //удаление - перемещаем последний элемент на удаляемый индекс, и обнуляем последний элемент
    {
     ABOOK[z].name[i]=ABOOK[count-1].name[i];                  ABOOK[count-1].name[i]=0;
     ABOOK[z].second_name[i]=ABOOK[count-1].second_name[i];    ABOOK[count-1].second_name[i]=0;
     ABOOK[z].tel[i]=ABOOK[count-1].tel[i];                    ABOOK[count-1].tel[i]=0;
     }
     count--; //стало на 1 элемент меньше

  } else {printf("Вы ввели неверный индекс");};
}

void PrintMenu()
{
printf("1) Добавить абонента\n");
printf("2) Удалить абонента\n");
printf("3) Поиск абонентов по имени\n");
printf("4) Вывод всех записей\n");
printf("5) Выход\n");
}

char * FindSTR(const char * str1,const char *str2)  //функция поиска подстроки из предыдущего задания
{
int i=0,j=0, L=0;
char *result=NULL;
L=STR_LENGTH-1; // допустимая индексация
for (i=0;i<STR_LENGTH;i++)
  for (j=0;j<STR_LENGTH;j++)
     { 
     if(*(str1+i)==0) return (NULL);// негде больше искать
     if (*(str2+j)==0) return (str1+i);    //ура, str2 закончилась, значит подстрока совпала
     if((i+j)>L) return (NULL);    //здесь ясно что подстрока не найдется
     if (*(str1+i+j)!=*(str2+j)) break; //не совпадение, идем дальше  
     }
}





