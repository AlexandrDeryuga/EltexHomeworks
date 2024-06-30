#include <stdio.h>

#define MAX 99    //кол-во записей в абонентской книге
#define STR_LENGTH 10

struct abonent {
 char name[STR_LENGTH];
 char second_name[STR_LENGTH];
 char tel[STR_LENGTH];
 } ABOOK[MAX];   //создадим уж наш справочник
 int count=0; // кол-во записей 

void PrintMenu();
unsigned char GetCommand();
void PrintItem(int item );
unsigned char TryAddItem();
char * FindSTR(const char * str1,const char *str2);  //функция поиска подстроки из предыдущего задания

enum MENU {BOOK_Add=1, BOOK_Delete,BOOK_Find, BOOK_PrintAll, BOOK_Exit} mymenu;


void main ()
{

int i=0;

while(mymenu!=BOOK_Exit)
 {
 PrintMenu();
 while (!GetCommand()) {};//процедура установит mymenu
 
 switch (mymenu)
 {case BOOK_Add:    TryAddItem();count++;PrintItem(count-1);break;
  case BOOK_Delete: break;
  case BOOK_Find:   break;
  case BOOK_PrintAll: 
                    if(count==0) {printf("Нет записей в справочнике");break;};
                    for(i=0;i<count;i++) PrintItem(i);                  
                    break; 
 };
 i++;
 }
}

void PrintItem(int item)
{if ((item>=0)&&(item<=count))
   printf("#%2d:%10s:%10s:%10s", item+1, ABOOK[item].name,ABOOK[item].second_name, ABOOK[item].tel);
}

unsigned char TryAddItem()
{
char scan1[STR_LENGTH], scan2[STR_LENGTH], scan3[STR_LENGTH];
if(count>=MAX) {printf("Справочник переполнен, удалите ненужных абонентов\n");return 0;};
printf("Введите информацию об абоненте (имя фамилия телефон):\n");
int L=STR_LENGTH;
int i=0;
int ch;

for (i=0; i<L;i++)// Инициализируем строки
{
scan1[i]=0;
sacn2[i]=0;
scan3[i]=0;
}

scanf("%s %s %s",&scan1,&scan2,&scan3);
printf("%s %s %s",&scan1,&scan2,&scan3);
//
/*
ch=1;
for (i=0; i<L;i++)
 while (ch!=10)               //Считываем Имя
    {ch=getchar();ABOOK[count].name[i]=ch;
    //if(ch!=32) {ABOOK[count].name[i]=i+48;} else break;
     //if(ch==10) return 0;
    }
   
   if(ch!=32)  while (32==getchar()) {};
   
for (i=0; i<L;i++)                  //Считываем Фамилию
  while (ch=getchar())
    {if(ch!=32) ABOOK[count].second_name[i]=ch;
     if(ch==10) return 0;
    }
 if(ch!=32)  while (32==getchar()) {};
 
 for (i=0; i<L;i++)                 //Считываем телефон
  while (ch=getchar())
    {if(ch!=32) ABOOK[count].tel[i]=ch;
     if(ch==10) return 1;
    }
 */
 printf("Запись:\n");
 count++;PrintItem(count);
}

unsigned char GetCommand()
{
unsigned char ch=0; 
printf("Ваш выбор (1-5):\n");
ch=getchar();//printf("%d\n",ch);
while (10!=getchar()) {ch=0;};
if((ch>=49)&&(ch<=53)) {mymenu=ch-48;return(mymenu);};
return 0;
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

