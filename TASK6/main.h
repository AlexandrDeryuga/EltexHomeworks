#ifndef MAIN_H
#define MAIN_H
void PrintMenu();            //выводит меню на экран
unsigned char GetCommand(); // Определяет какой из пунктов меню выбрал пользователь
void PrintItem(int item );  //печатает элемент структуры по индексу 0-MAX
unsigned char AddItem();    //Добавит нового абонента в справочник
void DeleteItem();          // удалить абонента из справочник
#endif




