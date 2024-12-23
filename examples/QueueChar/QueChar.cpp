/** Arduino, Esp32 ******************************************** QueChar.cpp ***
 * 
 *                          Обеспечить передачу и приём сообщений через очередь 
 *                                                   в задачах и из прерываниях
 * 
 * v1.0.0, 23.12.2024                                 Автор:      Труфанов В.Е.
 * Copyright © 2024 tve                               Дата создания: 23.12.2024
**/

#include "Arduino.h"

// Подключаем файлы обеспечения передачи и приёма сообщений через очередь 
#include "QueChar.h"

// ****************************************************************************
// *                  Построить объект (конструктор класса)                   *
// ****************************************************************************
TQue::TQue(int iQueueSize)
{
   // Определяем размер очереди из структур 
   QueueSize=iQueueSize;
}
// ****************************************************************************
// *                        Создать очередь сообщений                         *
// ****************************************************************************
String TQue::Create()
{
   // Инициируем успешное сообщение
   String inMess=tisOk;
   tQueue = xQueueCreate(QueueSize, sizeof(struct tStruMess));
   // Возвращаем ошибку "Очередь не была создана и не может использоваться" 
   if (tQueue==NULL) inMess=tQueueNotCreate; 

   strcopy1024("String Source");
   
   return inMess;
};

char* TQue::strcopy1024(String Source)
{
   const char* message_ptr = Source.c_str();
   int i=0;
   while(message_ptr[i]>0)
   {
      tBuffer[i]=message_ptr[i];
      i++;
      if (i>8) break;
   }
   tBuffer[i]=0;
   return tBuffer;
   
    
   //Serial.println(Source);
   /*
   int i=0;
   while((Source.c_str())[i]>0)
   {
      tBuffer[i]=(Source.c_str())[i];
      i++;
      if (i>1023) break;
   }
   tBuffer[i]=0;
   */
}
// ****************************************************************************
// *                          Отправить сообщение из задачи                   *
// ****************************************************************************
String TQue::Send(String Source)
{
   // Инициируем успешное сообщение
   String inMess=tisOk;
   // Если очередь создана, то отправляем сообщение в очередь
   if (tQueue!=0)
   {
      strcpy(taskStruMess.mess, Source.c_str());                              
      if (xQueueSend(tQueue,&taskStruMess,(TickType_t)0) != pdPASS)        
      {                                                                    
         sprintf(tBuffer,tFailSend);                                         
         inMess=String(tBuffer);                                           
      }
   }
   // Отмечаем "Отправка сообщения: очередь структур не создана!" 
   else inMess=tQueueNotSend;
   return inMess; 
}
String TQue::SendISR(String Source) 
{
   // Инициируем пустое сообщение
   String inMess=tisOk;
   // Если очередь создана, то отправляем сообщение в очередь
   if (tQueue!=0)
   {
      // Формируем сообщение для передачи в очередь
      strcpy(taskStruMess.mess, Source.c_str());                              
      // Сбрасываем признак переключения на более приоритетную задачу после прерывания 
      xHigherPriorityTaskWoken = pdFALSE;
      // Отправляем сообщение в структуре  
      if (xQueueSendFromISR(tQueue,&taskStruMess,&xHigherPriorityTaskWoken) != pdPASS)
      {
         // Если "Не удалось отправить структуру из прерывания!" 
         sprintf(tBuffer,tFailSendInrupt); 
         inMess=String(tBuffer);
      }
   }
   // Отмечаем "Отправка сообщения: очередь структур не создана!" 
   else inMess=tQueueNotSend;
   return inMess; 
}
// ****************************************************************************
// *        Подключить внешнюю функцию передачи сообщения на периферию        *
// ****************************************************************************
void TQue::attachFunction(void (*function)(char *mess, char *prefix)) 
{
   atatchedF = *function;  
}
// ****************************************************************************
// *            Определить, сколько сообщений накопилось в очереди            *
// *                            и их можно выгрузить                          *
// ****************************************************************************
int TQue::How_many_wait()                 
{
   // Инициируем отсутствие массива очереди
   int nMess = -1; 
   // Если очередь создана, то возвращаем количество сообщений в очереди
   if (tQueue!=NULL) nMess = int(uxQueueMessagesWaiting(tQueue)); 
   return nMess;     
}
// ****************************************************************************
// *              Определить количество свободных мест в очереди              *
// ****************************************************************************
int TQue::How_many_free() 
{               
   // Инициируем отсутствие массива очереди
   int Space = -1; 
   // Если очередь создана, то возвращаем количество свободных мест в очереди
   if (tQueue!=NULL) Space = int(uxQueueSpacesAvailable(tQueue)); 
   return Space;     
}
// ****************************************************************************
// *                              Принять сообщение                           *
// ****************************************************************************
char* TQue::Receive()
{
   // Принимаем сообщение
   if (tQueue!=NULL)
   {
      // Определяем сколько сообщений накопилось в очереди и их можно выгрузить              
      int nMess = How_many_wait();
      // Если есть сообщение в очереди, то выбираем одно сообщение
      // (без блокировки задачи при пустой очереди)
      if (nMess>0)
      {
         // Чистим буфер сообщения
         sprintf(tBuffer,""); 
         // Если сообщение выбралось из очереди успешно, то собираем его в буфер
         if (xQueueReceive(tQueue,&receiveStruMess,(TickType_t )0)==pdPASS) 
         {        
            strcat(tBuffer, receiveStruMess.mess);
         }
         // Иначе отмечаем, что "Ошибка при приёме сообщения из очереди"  
         else sprintf(tBuffer,tErrorReceiving); 
      }
      // Отмечаем, что "Очередь пуста при приёме сообщения" 
      else if (nMess==0) sprintf(tBuffer,tQueueEmptyReceive); 
      // "Не может быть!"
      else sprintf(tBuffer,"Не может быть: TQueMessage::Receive!");
   }
   // Отмечаем "Прием сообщения: очередь структур не создана"
   else sprintf(tBuffer,tNoQueueReceive); 
   return tBuffer; 
}
// ****************************************************************************
// *          Выбрать сообщение из очереди и отправить на периферию           *
// ****************************************************************************
char* TQue::Post(char *prefix)
{
   Receive(); 
   if (String(tBuffer)!=tQueueEmptyReceive) (*atatchedF)(tBuffer,prefix); 
   return tBuffer; 
}
// ****************************************************************************
// *      Выбрать все сообщения разом из очереди и отправить на периферию     *
// ****************************************************************************
void TQue::PostAll(char *prefix)
{
   int iwait=How_many_wait();
   while(iwait>0)
   {
      Receive(); 
      if (String(tBuffer)!=tQueueEmptyReceive) (*atatchedF)(tBuffer,prefix); 
      iwait=How_many_wait();
   }
}

/*
// Может пригодится!!!

// Сделать определитель типов:
#define tstr "tstr"
#define tchr "tchr"
#define tint "tint"
String types(String a) {return tstr;}
String types(char *a)  {return tchr;}
String types(int a)    {return tint;}

// Перевести массив char в String и обратно
void schastr()
{
   // Определяем структуру изменяемого сообщения
   struct AMessage
   {
      int  ucSize;        // Длина сообщения (максимально 256 байт)
      char ucData[256];   // Текст сообщения
   }  xMessage;
   
   String temp = "Всем привет!";
   strcpy(xMessage.ucData, temp.c_str());
   xMessage.ucSize = 0;
   while (xMessage.ucData[xMessage.ucSize]>0) 
   {
      xMessage.ucSize++;
   }
   Serial.println(temp);
   Serial.println(types(temp));
   Serial.println(xMessage.ucData);
   Serial.println(types(xMessage.ucData));
   Serial.println(xMessage.ucSize);
   Serial.println(types(xMessage.ucSize));
   
   String temp1=String(xMessage.ucData);
   Serial.println(temp1);
   Serial.println(temp1.length());
   Serial.println("-----");
}
*/






























/*
// ****************************************************************************
// *      Извлечь сообщение из массива по номеру и заполнить уточнениями      *
// ****************************************************************************
void TQueMessage::ExtractMess(String Source, int Number, String fmess32, String smess32) 
{
   sprintf(tMess,"Неопределенное сообщение примера очередей");
   for(int i=0; i<SizeMess; i++) 
   {
      if (i==Number)
      {
         // Выводим "простое сообщение, без уточнений"
         if (amessAPP[i].vmess==tvm_simpmes)
         { 
            sprintf(tMess,amessAPP[i].cmess); break;
         }
         // Выводим "сообщение c одним уточнением целого типа"
         if (amessAPP[i].vmess==tvm_1intmes) 
         {
            sprintf(tMess,amessAPP[i].cmess,fmess32); break;
         }
         // Выводим "сообщение c одним уточнением целого типа"
         if (amessAPP[i].vmess==tvm_2intmes) 
         {
            sprintf(tMess,amessAPP[i].cmess,fmess32,smess32); break;
         }
      }
   }
}
// ****************************************************************************
// *                              Собрать сообщение                           *
// ****************************************************************************
// Определить сколько символов без нуля в массиве char 
int TQueMessage::CharSize(char mess[])
{
   int nSize=0;
   while(mess[nSize]>0)
   {
      nSize++;
   }
   return nSize;
}
void TQueMessage::CollectMessage(int t_MessFormat)
{
   // char Type[7];                     - Тип сообщения
   // char Source[7];                   - Источник сообщения
   // int  Number;                      - Номер сообщения
   // char fmess32[32];                 - Первое уточнение сообщения
   // char smess32[32];                 - Второе уточнение сообщения

   // tfm_BRIEF,  0 Краткий             - WARNING-ISR[2]
   // tfm_NOTIME, 1 Без даты и времени  - WARNING-ISR[2] Управление передаётся планировщику
   // tfm_FULL,   2 Полный              - 2024-11-29,19:36:18 WARNING-ISR[2] Управление передаётся планировщику

   // Чистим буфер сообщения
   sprintf(tBuffer,""); 
   // Формируем краткое сообщение
   sprintf(tMess,""); 
   strcat(tMess, receiveStruMess.Type);
   strcat(tMess, "-");
   strcat(tMess, receiveStruMess.Source);
   strcat(tMess, "[");
   strcat(tMess, String(receiveStruMess.Number).c_str());
   strcat(tMess, "]");
   // Если заказан вывод кратких сообщений, то возвращаем сообщение
   if (t_MessFormat==tfm_BRIEF) strcat(tBuffer,tMess);
   else 
   {
      // Переделываем начало полного сообщения
      if (t_MessFormat==tfm_FULL)
      {
         // Если заказан вывод полных сообщений, то вытаскиваем дату и время  
         ExtractTime();
         sprintf(tBuffer,""); 
         strcat(tBuffer,dtime);
         strcat(tBuffer," ");
      }
      // Вкладываем фрагмент краткого сообщения
      strcat(tBuffer,tMess);
      // По источнику и номеру сообщения извлекаем контекст сообщения
      ExtractMess(String(receiveStruMess.Source),receiveStruMess.Number,String(receiveStruMess.fmess32),String(receiveStruMess.smess32));
      strcat(tBuffer, " ");
      // Определяем насколько заполнился буфер 
      int nFill=CharSize(tBuffer);
      // Переносим оставшееся/возможное число символов в буфер
      int i=0;
      while(tMess[i]>0)
      {
         tBuffer[nFill]=tMess[i];
         nFill++; i++;
         // Проверяем не вышли ли за максимальный размер буфера
         if (nFill>254) break;
      }
      tBuffer[nFill]=0;
   }
}
*/
// ************************************************************ QueChar.cpp ***
