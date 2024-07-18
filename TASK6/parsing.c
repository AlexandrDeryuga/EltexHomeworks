#include "parsing.h"
#include "dinamic.h"


extern abonent *ABOOK;    // Секция глобальных переменных
extern unsigned int MAX;


int ScanWord(int param, char *str, int L, int flag,int *ch)   //L - STRLENGTH
{

*ch=0;
int i=0;
*ch=getchar();
while (*ch=='\n') {*ch=getchar();}                            // scanf(%10s %10s %10s ) через гетчар
while (*ch==' ') {*ch=getchar();}
while (*ch!='\n') {
                  if((param)&&(*ch=='*')&&(i==0)) break; // если не строго, и * - то пропускаем
                  if(i>=L) {while ((*ch!='\n')&&(*ch!=' ')) {*ch=getchar();}; //если выход за границу поля, то отбрасываем остаток
                            break;};
                  if(*ch==' ') break;
                  if((*ch>=48)&&(*ch<=122)) // пропустим в обработку только цифры и английские литеры
                  {
                  *(str+i)=*ch;
                  i++;
                  } else {flag=-1;}
                  *ch=getchar();
                  }  
return flag; 
}


int  FillStructure(int Index,int param)// param=0 - строгий ввод аргументов
{int flag=3;// (1,2,3) - сколько прочитали слов, -1 - слово не прошло валидацию, 0 структура не заполнена
 int i=0; 
 int ch=0;

flag=ScanWord(param,ABOOK[Index].name, STR_LENGTH,flag,&ch);  
if(ch=='\n') return 1; 

flag=ScanWord(param,ABOOK[Index].second_name, STR_LENGTH,flag,&ch);
if(ch=='\n') return 2;

flag=ScanWord(param,ABOOK[Index].tel, STR_LENGTH,flag,&ch);  
                  //по итогу должны иметь аккуратно заполненную структуру либо флаг неверной раскладки

if(ch=='*') ch=getchar();
if(ch!='\n'){ //введено  более чем 3 параметра
              printf("введено  более чем 3 параметра\n");
             while(ch!='\n') ch=getchar(); //считать мусор
             return -1; 
             };                     

  if((param)&&(0==ABOOK[Index].tel[0])&&(0==ABOOK[Index].name[0])&&(0==ABOOK[Index].second_name[0])) return 0; // структура не заполнена, для поиска не введены параметры


return flag;// 
}

void FindAbonent()
{int i=0,findcount=0, fill=0;
static char *A=NULL; //Массив указывающий подходит ли  из списка 
A=DinamicFilterIndexActual(MAX);//актуализировать размер массива индекса, укажем !0  что бы выделить память
if(NULL==A) {printf("Список абонентов пуст");return;}

 
printf("Введите имя фамилию или номер телефона абонента для поиска:\n");
printf("формат(Английская раскладка): Имя/* Фамилия/* телефон/* \n");
i=0;
while(i!=3)
 {
 i=FillStructure(MAX,1); // Заполним строки, по которым будем искать, параметр =1 допускает * (не заполнение какого либо поля структуры) 
 
 
 if(i==0) {printf("не задан фильтр\n");continue;} // введено было 3 звездочки * * *   
 if(i==-1) {printf("не верный формат\n");continue;}; 
 
 //if((i==1)||(i==2)) break; // Допускаем ввод не всех трех значений, более верно следующее:
 fill=0; //определим сколько в реальности полей заполнено
 if(0!=ABOOK[MAX].tel[0]) fill++;
 if(0!=ABOOK[MAX].name[0]) fill++;
 if(0!=ABOOK[MAX].second_name[0]) fill++;
 if((fill>0)&&(fill<=3)) break;
 printf("повторите ввод:\n");
   for(i=0;i<STR_LENGTH;i++)    //обнулим попытку ввода
       {ABOOK[MAX].name[i]=0;
        ABOOK[MAX].second_name[i]=0;
        ABOOK[MAX].tel[i]=0;
      }
 }


  printf("ищем совпадения по фильтру, определено %d поле для поиска:\n", fill);
  PrintItem(MAX);
  printf("================================================\n");

   char *x=NULL;
   
  if(0!=ABOOK[MAX].name[0]) //найдем здесь записи подходящие для нашего фильтра
     {for(i=0;i<MAX;i++) {x=FindSTR(ABOOK[i].name,ABOOK[MAX].name,STR_LENGTH);
                         if(x!=NULL) A[i]+=1;
                         x=NULL;
                         }
      }
  if(0!=ABOOK[MAX].second_name[0]) //найдем здесь записи
     {for(i=0;i<MAX;i++){ x=FindSTR(ABOOK[i].second_name,ABOOK[MAX].second_name,STR_LENGTH);
                        if(NULL!=x) A[i]+=1;
                        x=NULL;
                        }
    }
   if(0!=ABOOK[MAX].tel[0]) //найдем здесь записи
     {for(i=0;i<MAX;i++) {x=FindSTR(ABOOK[i].tel,ABOOK[MAX].tel,STR_LENGTH);
                          if(NULL!=x) A[i]+=1;
                          x=NULL;
                        }
     }
     
  for(i=0;i<MAX;i++) if(A[i]==fill) findcount++; // запись нам подходит, если все введенные параметры совпадают
  printf("найдено %d записей по фильтру:\n", findcount);  // 
  printf("================================================\n");
  for(i=0;i<MAX;i++) if(A[i]==fill) PrintItem(i);
  
    
    for(i=0;i<STR_LENGTH;i++) 
       {ABOOK[MAX].name[i]=0;
        ABOOK[MAX].second_name[i]=0;
        ABOOK[MAX].tel[i]=0;
      }
  // DinamicFilterIndexActual(0); //освободим динамическую память, а лучше сделаем статический указатель
}


char * FindSTR(const char * str1,const char *str2,int L)  //функция поиска подстроки из предыдущего задания
{
int i=0,j=0;
char *result=NULL;
if(L>0) 
{L=L-1; // допустимая индексация
for (i=0;i<STR_LENGTH;i++)
  for (j=0;j<STR_LENGTH;j++)
     { 
     if(*(str1+i)==0) return (NULL);// негде больше искать
     if (*(str2+j)==0) return (str1+i);    //ура, str2 закончилась, значит подстрока совпала
     if((i+j)>L) return (NULL);    //здесь ясно что подстрока не найдется
     if (*(str1+i+j)!=*(str2+j)) break; //не совпадение, идем дальше  
     }
 }
}





