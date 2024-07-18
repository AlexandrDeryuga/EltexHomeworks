
#include "main.h"
#include "parsing.h"
#include "dinamic.h"

extern abonent *ABOOK;    // Секция глобальных переменных
extern unsigned int MAX;
extern unsigned int count;

#define R_CASH 1


enum MENU {BOOK_Add=1, BOOK_Delete,BOOK_Find, BOOK_PrintAll, BOOK_Exit} mymenu;


void main ()
{

int i=0,j=0,z=0;

//z=getchar(); printf("%d",z);

/*for(i=0;i<=MAX;i++) for(j=0;j<STR_LENGTH;j++) //Инициализация справочника
  {
   ABOOK[i].name[j]=0;
   ABOOK[i].second_name[j]=0;
   ABOOK[i].tel[j]=0;
  }
  */
  
  DinamicAddElement(1,R_CASH,1); // Выделим память под парсинг, 1- кол-во ячеек памяти
  //но в этом случае надо не допускать освобождение памяти вместе с парсинг строкой либо обнулять Reccount.
    
while(mymenu!=BOOK_Exit)
 {
 //if (MAX<=0) DinamicAddElement(1,R_CASH); // Выделим память под парсинг, 1- кол-во ячеек памяти
 PrintMenu();
  #ifdef _DEBUG
     printf("\nMAX %d count %d\n",MAX,count);// удобно наблюдать за работой программы с точки зрения динамичеких массивов
    #endif
 
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
 DinamicFilterIndexActual(0); //освободим память
 if(ABOOK) free(ABOOK);
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
    int trace=DinamicAddElement(count+1,R_CASH,0); // Выделим память, нам нужно count (под абонентов) +1 под парсинг
   
     if(trace==-1) {printf("Память не может быть выделенна\n");
     count--;
     for(i=0;i<STR_LENGTH;i++) 
       {ABOOK[count].name[i]=0;
        ABOOK[count].second_name[i]=0;
        ABOOK[count].tel[i]=0;
       } 
    return 0;     }
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
     
     int trace= DinamicDeleteElement(count,R_CASH);// высвобождаем память - а именно говорим сколько памяти нужно
                                                   // (кол-во абонентов-1) +1 запись на парсинг  = count (еще не уменьшенный)
    
     
     if(trace==-1) {printf("Ошибка при работе с динамической памятью - Delete\n");}
     count--; //стало на 1 элемент меньше
     
  } else {printf("Вы ввели неверный индекс\n");};
}

void PrintMenu()
{
printf("\n1) Добавить абонента\n");
printf("2) Удалить абонента\n");
printf("3) Поиск абонентов по имени\n");
printf("4) Вывод всех записей\n");
printf("5) Выход\n");
}






