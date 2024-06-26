#include <stdio.h>

void printD(unsigned long scanD)
{
unsigned long bit=0,mask=1,count=0;  //число для анализа, и переменная для определения значения бита,  маска,кол-во единичек
unsigned char byte=0;

mask=mask<<31;//определим маску, нам нужна единичка в самом левом бите

for(int k=1;k<=4;k++) //кол-во байтов в числе, ну и 8 битов в байте
{
 for(int i=1;i<=8;i++)    //работаем байт, берем по маске крайний бит, печатаем , сдвигаем
 {
//bit=scanD&mask ? 1 : 0; // если по  маске не ноль, то 1
bit=(scanD&mask)>>31; // так быстрее
count+=bit;
printf("%ld",bit);
scanD=scanD<<1;
 }

printf(" ");
}

printf ("\nкол-во единиц в бинарном представлении %ld \n", count);


}


void main ()
{
unsigned long scan=0,mask=0;
unsigned char byte;

printf("введите целое 4 байтное число (максимально отрицательное -2147483648):");
int n=scanf("%ld", &scan);  // scan число беззнаковое, но отрицательное число сканируется  корректно
printf("\nвведите значение третьего байта, (не более 255):");
scanf("%ud",&byte);

printf("\nСчитано %d символов. число для обработки: %ld",n, scan);

printf("\nбитовое представление введенного числа:\n");
printD(scan); // распечатаем биты введенного числа

mask=byte; //привели в структуру маски для замены
mask=mask<<16; //позиционировали маску на 3 байт

printf("\nМаска:\n");
printD(mask);

scan=scan|mask; //замена 3 байта в числе
printf("\nЧисло после замены\n");
printD(scan);

}

