/** Arduino, Esp32 ********************************************** QueChar.h ***
 * 
 *     Обеспечить передачу и приём сообщений "как есть", максимум 1023 символа, 
 *               плюс завершающий ноль, через очередь в задачах и из прерываний
 * 
 * v1.0.0, 23.12.2024                                 Автор:      Труфанов В.Е.
 * Copyright © 2024 tve                               Дата создания: 23.12.2024
**/

#pragma once     
#include "Arduino.h"

// Передатчик сообщения на периферию с возможным префиксом (по умолчанию):
inline void transQue(char *mess, char *prefix="") 
{
   // Выводим массивы символов с 0-вым окончанием
   Serial.print(prefix);  // передали префикс (по умолчанию отсутствует)
   Serial.println(mess);  // передали сообщение
}
// Пример передачи сообщения из очереди через передатчик на периферию:
//    static char str[] = "Hello: ";
//    queMessa.Post(queMessa.Receive(MessFormat),str);

// Сообщения из методов по обслуживанию очередей вне формата 
// (в стандартный последовательный порт или в прочие периферии)
#define tisOk               "Всё хорошо" 
#define tNoQueueReceive     "Прием сообщения: очередь структур не создана"            
#define tQueueEmptyReceive  "Очередь пуста при приёме сообщения"               
#define tErrorReceiving     "Ошибка при приёме сообщения из очереди"             
#define tQueueNotCreate     "Очередь не была создана и не может использоваться"  
#define tQueueBeformed      "Очередь сформирована"                               
#define tFailSendInrupt     "Не удалось отправить структуру из прерывания"       
#define tQueueNotSend       "Отправка сообщения: очередь структур не создана"   
#define tFailSend           "Очередь занята, не удалось отправить сообщение"   

// Определяем структуру передаваемого сообщения
struct tStruMess
{
   char mess[1024]; 
};

class TQue
{
   public:

   // Построить объект (конструктор класса)
   TQue(int iQueueSize=4);
   // Создать очередь
   String Create();
   // Отправить сообщение из задачи
   String Send(String Source); 
   // Отправить сообщение из прерывания
   String SendISR(String Source);
   // Подключить внешнюю функцию передачи сообщения на периферию       
   void attachFunction(void (*function)(char *mess, char *prefix));
   // Определить количество свободных мест в очереди
   int How_many_free();                
   // Определить, сколько сообщений накопилось в очереди и их можно выгрузить 
   int How_many_wait(); 
   // Выбрать сообщение из очереди
   char* Receive();
   // Выбрать сообщение из очереди и отправить на периферию 
   char* Post(char *prefix="");
   // Выбрать все сообщения разом из очереди и отправить на периферию
   void PostAll(char *prefix="");

   private:

   void (*atatchedF)(char *mess, char *prefix); // прикреплённая функция
   int QueueSize;                               // размер очереди 
   struct tStruMess taskStruMess;               // структура для для отправки сообщения 
   struct tStruMess receiveStruMess;            // структура для для приема сообщения 
   char tBuffer[1024];                          // буфер сообщения на 1023 символа и завершающий ноль
   QueueHandle_t tQueue;                        // очередь (дескриптор) сообщений из структур tStruMess1024   
   // Выделить переменную планировщику задач FreeRTOS для указания
   // необходимости переключения после прерывания на более приоритетную 
   // задачу, связанную с очередью
   BaseType_t xHigherPriorityTaskWoken;
   // Скопировать не более 1023 символов сообщения в буфер и завершить нулем
   void strcopy1024(String Source);            
};

// ************************************************************** QueChar.h ***