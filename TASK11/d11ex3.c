#include "main.h"


Shop Shops[NUM_SHOPS];
Buyer Buyers[NUM_BUYERS];
Load ShopsLoader;

char SStatus[2][50];
int AvailableShops=NUM_SHOPS;
int ClientsCount=NUM_BUYERS;

pthread_t tid[NUM_THREADS];      /* array of thread IDs */
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

   

int main()
{
    int i=0;
    unsigned long Need_All=0;
    unsigned long Sold_All=0;
    unsigned long Load_All=0;
    unsigned long TMPSUM=0;
    int loadscount=0;
    
   InitShops(0, "Магазин1",0,10000,0.00001);//инициилизировать Магазин элемент случайным числом от А до B
   InitShops(1, "Магазин2",0,10000,0.00001);
   InitShops(2, "Магазин3",0,10000,0.00001);
   InitShops(3, "Магазин4",0,10000,0.00001);
   InitShops(4, "Магазин5",0,10000,0.00001);
 
   strcpy(SStatus[1],"СВОБОДЕН");  //подвязка к Shop.Free
   strcpy(SStatus[0],"ЗАНЯТ");
   
   LoaderInit(5000,1,0.0001);
  
   
   
   InitBuyers(0, "Покупатель1",70000,200000,2);  //инициилизировать Покупателя элемент случайным числом от А до B и временем блокировки магазина
   InitBuyers(1, "Покупатель2",70000,200000,2);
   InitBuyers(2, "Покупатель3",70000,200000,2);
   
   for(i=0;i<NUM_SHOPS;i++) {Load_All+=Shops[i].Product;
                             printf("%s товара %lu, время обслуживания покупателя на ед товара= %f\n",Shops[i].Name,Shops[i].Product,Shops[i].ClientTime);
                            }
   for(i=0;i<NUM_BUYERS;i++) {Need_All+=Buyers[i].Need;
                              printf("%s потребность %lu, Стационарное время блокировки магазина  %f\n",Buyers[i].Name,Buyers[i].Need,Buyers[i].SleepTime);
                             }
   printf("===========================================================================================\n");
   printf("Загружено в магазины: %lu , Общая потребность %lu !\n", Load_All,Need_All);
   printf("Параметры погрузчика:   время реакции (SleepTime %f   Время погрузки =%ld*%f =%f\n",ShopsLoader.SleepTime,ShopsLoader.Load_Part,ShopsLoader.LoadTime, ShopsLoader.Load_Part*ShopsLoader.LoadTime );
   printf("===========================================================================================\n");
   printf("Главная - создаю потоки!\n");
     
    clock_t start, end;
    long double elapsed;
    start = clock();
                             
                             //end = clock();
                           //  elapsed=((double)(end-start))/CLOCKS_PER_SEC;

    
    
        for ( i = 0; i < AvailableShops; i++)
            pthread_create(&tid[i], NULL, (void*)ShopFunc, (void*)&Shops[i]);  // Создадим потоки магазинов, так можно потому что не i передаем,
                                                                        // а данные зависимые от i, но проверить это
        for ( i = 0; i < ClientsCount; i++)
            pthread_create(&tid[AvailableShops+i], NULL, (void*)BuyerFunc,(void*)&Buyers[i]);  // Создадим потоки магазинов
            
             pthread_create(&tid[AvailableShops+i+1], NULL, (void*)LoaderFunc,(void*)&ShopsLoader);  //Погрузчик  
             
      
       for ( i = 0; i < NUM_THREADS; i++)
            pthread_join(tid[i], NULL);
        
        end = clock();
        elapsed=((double)(end-start))/CLOCKS_PER_SEC;;
        
        printf("Все %d нитей завершились, время работы: %Lf  сек.\n", i, elapsed);
        
         for ( i = 0; i < NUM_SHOPS; i++) 
          {printf("%s , товаров %ld, продано товаров: %ld, всего погрузок %d\n",Shops[i].Name, Shops[i].Product,Shops[i].Sold, Shops[i].StatLoadsCount);
           Sold_All+=Shops[i].Sold;
           loadscount+=Shops[i].StatLoadsCount;
                                   
                                   
           }
                                   
         for ( i = 0; i < 3; i++) {
                                   printf("%s , потребность %ld\n",Buyers[i].Name, Buyers[i].Need);
                                   printf("Моя статистика:\n");
                                   for (int j=0;j<NUM_SHOPS;j++) printf("%s покупок %d\n",Shops[j].Name, Buyers[i].ShopsStat[j]);
                                   }
        printf("Всего продано : %lu,  Всего потребность:%lu\n",Sold_All,Need_All);
        printf("Всего погрузок в магазины : %d,  Всего погрузок погрузчиком:%ld\n",loadscount,ShopsLoader.Load_All);
                        
        
    
    exit(0);
}

/*
инициилизировать Магазин элемент случайным числом от А до B
и присвоить ему имя
и время обслуживания покупателя на единицу товара
*/
void InitShops(int index, char* name,unsigned long A,unsigned long B, float clienttime)
{
strcpy(Shops[index].Name, name);
Shops[index].Free=1;
Shops[index].Product=A + rand()%(B-A+1);
strcpy(Shops[index].Status_ClientName," ");
Shops[index].Changed=1; //флаг того что можно печатать о себе информацию

Shops[index].StatClientsCount=0;
Shops[index].StatLoadsCount=0;
Shops[index].ClientTime=clienttime;

}

void InitBuyers(int index, char* name,unsigned long A,unsigned long B, float sleptime)
{
strcpy(Buyers[index].Name,name);
Buyers[index].Need=A + rand()%(B-A+1);
Buyers[index].Changed=1; //флаг того что можно печатать о себе информацию
for (int i=0;i<NUM_SHOPS;i++) Buyers[index].ShopsStat[i]=0;
Buyers[index].SleepTime=sleptime;
}

void LoaderInit(int part, float sleep, float loadtime)
{  ShopsLoader.Load_Part=part;
   ShopsLoader.Load_All=0;
   ShopsLoader.SleepTime=sleep;
   ShopsLoader.LoadTime=loadtime;
   }





void *ShopFunc(Shop *Shp)
{
    unsigned long id;
   
    errno = 0;
    id = syscall(SYS_gettid);
    if(0 != errno)   { perror("syscall(SYS_gettid) failed");abort();}
    
    printf("[%lu] Я %s, Начинаю обслуживание клиентов. Время обслуживания клиентов на ед товара %f\n",id, Shp->Name, Shp->ClientTime);
    
    while (ClientsCount >0) 
       {
       if(Shp->Changed) printf("[%lu] Я %s, товара на складе в кол-ве %lu шт, мой статус: %s %s\n", id, Shp->Name,Shp->Product,SStatus[Shp->Free],Shp->Status_ClientName);
      
        
        pthread_mutex_lock( &mutex ); 
        Shp->Changed=0;
        pthread_mutex_unlock( &mutex );
      
       
        };
   
   printf("[%lu] %s Больше нет клиентов, закрываюсь (.)\n", id, Shp->Name);
   
   pthread_mutex_lock( &mutex ); 
   AvailableShops--;
   Shp->Free=0;
   pthread_mutex_unlock( &mutex );
   
}

int FindFreeShop(Buyer *B,int startindex)//B==NULL - тогда ищет погрузчик
{int i=0;
if(startindex==-1) startindex=0;
if(B==NULL){                                     //Код погрузчика, его задача равномерно насыщать магазины, разобьем цикл на 2
            for(i=startindex;i<AvailableShops;i++)
              if (Shops[i].Free) 
                                  {Shops[i].Free=0;  // занимаем магазин
                                   strcpy(Shops[i].Status_ClientName,"Идет погрузка..");
                                   Shops[i].Changed=1;
                                    return i;}
             if(startindex)  for(i=0;i<startindex;i++) 
                 if (Shops[i].Free)
                                   {Shops[i].Free=0;  // занимаем магазин
                                   strcpy(Shops[i].Status_ClientName,"Идет погрузка..");
                                   Shops[i].Changed=1;
                                    return i;}
             
            return -1;
           }
           
           
for(i=0;i<AvailableShops;i++)                  //Код магазина, задача клиента удовлетворить свою потребность, 
if (Shops[i].Free && (Shops[i].Product>0))     // и по хорошему здесь мы должны были выбрать не первый попавшийся,  а с максимальным кол-вом товара
  {                                            // но выберем первый
   Shops[i].Free=0;
   strcpy(Shops[i].Status_ClientName,B->Name); //занимаем магазин
   Shops[i].Changed=1;
   return i;};
return -1;
}

inline void FreeShop(int i)
{
Shops[i].Free=1;
strcpy(Shops[i].Status_ClientName," ");
Shops[i].Changed=1;
}

void *BuyerFunc(Buyer *B)              //покупатель, и здесь же вся математика,  маназин скорее нужен как объект отслеживающий изменения над собой
{
    unsigned long id,tmp;
    int ShopIndex=0;
    errno = 0;
    id = syscall(SYS_gettid);
    if(0 != errno)   { perror("syscall(SYS_gettid) failed");abort();}
    
    while (B->Need >0) 
       {
       if(B->Changed) printf("[%lu] Я %s, потребность в товаре %lu шт, мой статус: Ищу свободный магазин\n", id, B->Name,B->Need);
       
         
       ShopIndex=FindFreeShop(B,0);  //поиск с нуля
       if(ShopIndex==-1) {                  //свободный магазин с товаром не найден.
                          if(B->Changed) printf("[%lu] Я %s, потребность в товаре %lu шт, мой статус: Нет свободного магазина, жду\n", id, B->Name,B->Need);
                          B->Changed=0;
                          continue;
                          } else 
                              { B->Changed=1;
                                B->ShopsStat[ShopIndex]+=1; //запомним в какой магазин мы заходили
                              
                              }
       
        pthread_mutex_lock( &mutex ); 
        printf("[%lu] Я %s, потребность в товаре %lu шт, мой статус: Найден  свободный магазин %s, товаров %lu отгружаю\n", id, B->Name,B->Need,Shops[ShopIndex].Name, Shops[ShopIndex].Product);
        if(B->Need > Shops[ShopIndex].Product)   // Здесь моделируется обслуживание клиента магазином, 
             { 
              sleep(Shops[ShopIndex].Product*Shops[ShopIndex].ClientTime);
              Shops[ShopIndex].Sold+=Shops[ShopIndex].Product;
              B->Need=B->Need-Shops[ShopIndex].Product;
              Shops[ShopIndex].Product=0;
              
             }
             else
               {
               sleep(B->Need*Shops[ShopIndex].ClientTime);
               Shops[ShopIndex].Sold+=B->Need;
               Shops[ShopIndex].Product-=B->Need;
               B->Need=0;
               }
        pthread_mutex_unlock( &mutex );
         B->Changed=0;
        sleep(B->SleepTime);
        pthread_mutex_lock( &mutex );
        FreeShop(ShopIndex);
        pthread_mutex_unlock( &mutex );      
         
        //pthread_mutex_unlock( &mutex );
       // B->Changed=0;
       
        };
   
   printf("[%lu] %s Потребность закрыта, ухожу с рынка (.)\n", id, B->Name);
   pthread_mutex_lock( &mutex );
   ClientsCount--;
   pthread_mutex_unlock( &mutex );
   
}


void *LoaderFunc(Load *L)
{
    unsigned long id,tmp;
    int ShopIndex=0;
    errno = 0;
    id = syscall(SYS_gettid);
    if(0 != errno)   { perror("syscall(SYS_gettid) failed");abort();}
    
    while (AvailableShops >0) 
       {
       printf("[%lu] Я - Погрузчик, ищу свободный магазин.  Магазинов: %d, Клиентов %d\n", id, AvailableShops, ClientsCount);
        
       ShopIndex=FindFreeShop(NULL,ShopIndex);
       if(ShopIndex==-1) { 
                          printf("[%lu] Я - Погрузчик, нет свободных магазинов\n",id);//свободный магазин с товаром не найден.
                          sleep(1);
                          continue;} 
       
      
        printf("[%lu] Я - Погрузчик: Найден  свободный магазин %s, товаров %lu отгружаю %ld ед. товара\n", id,Shops[ShopIndex].Name,Shops[ShopIndex].Product,L->Load_Part);
        pthread_mutex_lock( &mutex );    
        Shops[ShopIndex].Product+=L->Load_Part;
        pthread_mutex_unlock( &mutex );
        
        sleep(L->Load_Part*L->LoadTime); //время погрузки
        printf("[%lu] Я -Погрузчик, мой статус: погрузку закончил, %s, товара стало  %lu ед. \n", id,Shops[ShopIndex].Name,Shops[ShopIndex].Product);
        pthread_mutex_lock( &mutex );
        Shops[ShopIndex].StatLoadsCount+=1;
        L->Load_All+=1;
        FreeShop(ShopIndex);
        pthread_mutex_unlock( &mutex );
        
        sleep(L->SleepTime);//Время реакции
        };
   
   printf("[%lu] Больше нет магазинов, завершаюсь.\n", id);
   
}
