/** Arduino, Esp32 ********************************************** Que1024.h ***
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
inline void transQue1024(char *mess, char *prefix="") 
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
struct tStruMess1024
{
   char mess[1024]; 
};

class TQue1024
{
   public:

   // Построить объект (конструктор класса)
   TQue1024(int iQueueSize=4);
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
   struct tStruMess1024 taskStruMess;           // структура для для отправки сообщения 
   struct tStruMess1024 receiveStruMess;        // структура для для приема сообщения 
   char tBuffer[1024];                          // буфер сообщения на 1023 символа и завершающий ноль
   QueueHandle_t tQueue;                        // очередь (дескриптор) сообщений из структур tStruMess1024   
   /*
   char tMess[256];                             // буфер предварительного размещения контекста сообщения
   char dtime[20];                              // буфер даты и времени
   String SourceMessage;                        // источник сообщения
   String EmptyMessage="";                      // пустое сообщение
   tmessAPP* amessAPP;                          // указатель на массив сообщений
   int SizeMess;                                // размер массива сообщений
   // Выделяем переменную планировщику задач FreeRTOS для указания
   // необходимости переключения после прерывания на более приоритетную 
   // задачу, связанную с очередью
   BaseType_t xHigherPriorityTaskWoken;
   // Извлечь сообщение по источнику перечисления и номеру сообщения
   void ExtractMess(String Source,int Number,String fmess32,String smess32);
   // Извлечь информацию о текущем времени в отформатированном виде 
   void ExtractTime();
   // Собрать сообщение
   void CollectMessage(int t_MessFormat);
   // Определить сколько символов без нуля в массиве char 
   int CharSize(char mess[]);
   */
};

// ************************************************************** Que1024.h ***
