/*#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif*/

#ifndef MAIN_H
#define MAIN_H

#include <unistd.h>
#include <sys/syscall.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <pthread.h>


#define NUM_SHOPS 5
#define NUM_BUYERS 3

#define NUM_THREADS 9    //кол-во потоков, 5 магазинов, 3 покупателя, 1 погрузчик

typedef struct Shop
{                          //структура данных по магазину, (!) пометим те которые важно менять через мьютекс
char Name[50];
int Free;                  //!  в принципе только это и критично, поскольку если флаг установлен в 0, никто другой обрабатывать эти данные больше не будет 
unsigned long Product;     
unsigned long Sold;        // сколько продано
char Status_ClientName[50];      //кем занят
unsigned int StatClientsCount;   //сколько клиентов обслуженно, 
unsigned int StatLoadsCount; 
float ClientTime;                //время обслуживания клиента на еденицу товара - добавим в модель для экспериментов
int Changed;                     //! тоже лучше под мьютекс но не критично, флаг того что можно печатать о себе информацию
}  Shop;

typedef struct Buyer
{
char Name[50];
unsigned long Need;              //потребность
int Changed;                    //флаг того что можно печатать о себе информацию
int ShopsStat[NUM_SHOPS];       // статистика - в какие магазины обращался
float SleepTime;                // статическое время блокировки магазина
} Buyer;

typedef struct Load
{
unsigned long Load_Part; // Сколько грузим за раз
unsigned long Load_All;  // сколько всего погружено
float LoadTime;          // добавим в модель время погрузки на еденицу товара
float SleepTime;         // переодичность погрузки
} Load;

void *ShopFunc(Shop *);    /* thread    просто печатаем инфу о себе если было изменение состояния из вне*/        
void *LoaderFunc(Load *L); /* thread    погрузчик - задача равномерно насытить магазины*/
void *BuyerFunc(Buyer *B); /* thread    покупатель - задача насытиться самому */

int FindFreeShop(Buyer *B, int startindex);  // поиск свободного магазина начиная с индекса

void FreeShop(int i);  // inline освобождаем магазин, чистим атрибут кем занят, говорим что состояние изменилось

//ФУНКЦИИ УЛОБНОЙ ИНИЦИАОИЗАЦИИ
/*
index - номер магазина в его массиве
инициилизировать Магазин/покупатель элемент случайным числом от А до B
и присвоить ему имя
и время обслуживания покупателя на единицу товара/либо статическое время обслуживания покупателя
*/
void InitShops(int index, char* name,unsigned long A,unsigned long B, float clienttime); 
void InitBuyers(int index, char* name,unsigned long A,unsigned long B, float sleptime);

void LoaderInit(int part, float sleep, float loadtime);  //кол-во погрузки, переодичность, время блокировки магазина на единицу товара 

#endif

