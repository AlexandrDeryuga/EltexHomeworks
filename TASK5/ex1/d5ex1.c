/* Написать код в парадигме функций
код уже изначально был написан в парадигме функций, но можно кое что оптимизировать:

1.разобьем по модулям 

1.1 parsing.h и parsing.c
    Модуль фильтра - те функции которые будут отвечать за ввод строки, ее парсинг и валидацию, 
    и конвертацию ее в служебный элемент массива
1.2 вынести в функцию код для чтения слова из строки ввода 
1.3 Допустить в фильтре не обязательный ввод , конструкии отличные от 
    name/* second/* tel/*
    теперь можно писать сокращенные фильты типа (name, name/* second_name )
    
2. main.h -заголовки для функционала главного меню

3. так же сделаем пока простой makefile,  и научимся - как избегать ошибок переопределения
*/


#include "main.h"
#include "parsing.c"


/*                // Это вынесем в модуль парсинг из=-за подвязки к define  !!!!!
//#define MAX 3    //кол-во записей в абонентской книге
//#define STR_LENGTH 10

struct abonent {
 char name[STR_LENGTH];
 char second_name[STR_LENGTH];
 char tel[STR_LENGTH];
 } ABOOK[MAX+1];   //создадим уж наш справочник, последняя запись используется для фильтра 
// int count=0; // кол-во записей 
*/


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

void PrintItem(int item)            // функция печати структуры
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


unsigned char AddItem()                   // 
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
      return 0;
    }
    return 1;  //флаг успеха
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

  } else {printf("Вы ввели неверный индекс\n");};
}

void PrintMenu()
{
printf("1) Добавить абонента\n");
printf("2) Удалить абонента\n");
printf("3) Поиск абонентов по имени\n");
printf("4) Вывод всех записей\n");
printf("5) Выход\n");
}






