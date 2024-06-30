#include <stdio.h>
//#include <iostream>  //нет такого 
//#include <iostream.h> // нет такого 


#define STR_LENGTH 1000

char * FindSTR(const char * str1,const char *str2)
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

void main ()
{
int A[10];
int *ptr=NULL;

for(int i=0;i<10;i++) A[i]=i+1;//допустим есть массив, создали массив как обычно

ptr=&A[0];
printf ("Элементы массива: ");
for(int i=0;i<10;i++) printf("%d ", *(ptr+i));
printf("\n");
//std::cout << "A[" << i << "]="<< std::endl;


char str1[STR_LENGTH];
char str2[STR_LENGTH];
char *str3=NULL;

for(int i=0;i<STR_LENGTH;i++) {str1[i]=0;str2[i]=0;};

printf("Введите строку:");
gets(str1);
printf("Введите подстроку:");
gets(str2);
printf("Введенная строка:%s\n",str1);
printf("Введенная подстрока:%s\n",str2);

str3=FindSTR(str1,str2);
printf("Результат:");
if(str3!=NULL) printf("%s\n",str3);
if(str3==NULL) printf("Подстрока не найдена\n");

}

