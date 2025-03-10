/** Arduino, Esp32 ***************************************** QueMessage.cpp ***
 * 
 *                          Обеспечить передачу и приём сообщений через очередь 
 *                                                    в задачах и из прерываний
 * 
 * v3.3.0, 09.03.2025                                 Автор:      Труфанов В.Е.
 * Copyright © 2024 tve                               Дата создания: 29.11.2024
**/

#include "Arduino.h"

// Подключаем файлы обеспечения передачи и приёма сообщений через очередь 
#include "QueMessage.h"

// ****************************************************************************
// *                  Построить объект (конструктор класса)                   *
// ****************************************************************************
TQueMessage::TQueMessage(tmessAPP *aimessAPP, int iSizeMess, String itmk_APP, int iQueueSize)
{
   // Считывем указатель массива сообщений и размер массива
   amessAPP = aimessAPP;
   SizeMess = iSizeMess;
   // Определяем размер очереди из структур 
   QueueSize=iQueueSize;
   // Определяем источник сообщения
   if (itmk_APP.length()>7) SourceMessage=itmk_APP.substring(0,7);
   else SourceMessage=itmk_APP;
}
// ****************************************************************************
// *        Подключить внешнюю функцию передачи сообщения на периферию        *
// ****************************************************************************
void TQueMessage::attachFunction(void (*function)(char *mess, char *prefix)) 
{
   atatchedF = *function;  
}
// ****************************************************************************
// *                        Создать очередь сообщений                         *
// ****************************************************************************
String TQueMessage::Create()
{
   // Инициируем успешное сообщение
   String inMess=isOk;
   tQueue = xQueueCreate(QueueSize, sizeof(struct tStruMessage));
   // Возвращаем ошибку "Очередь не была создана и не может использоваться" 
   if (tQueue==NULL) inMess=QueueNotCreate; 
   return inMess;
};
// ****************************************************************************
// *                Сформировать простое сообщение, без уточнений             *
// ****************************************************************************
void TQueMessage::simpStruMess(String Type, int Number)
{
   // Заполняем структуру
   strcpy(taskStruMess.Type, Type.c_str());                                
   strcpy(taskStruMess.Source, SourceMessage.c_str());  
   taskStruMess.Number=Number;                                           
   strcpy(taskStruMess.fmess32, EmptyMessage.c_str());                   
   strcpy(taskStruMess.smess32, EmptyMessage.c_str()); 
};    
// ****************************************************************************
// *            Сформировать сообщение c одним уточнением целого типа         *
// ****************************************************************************
void TQueMessage::f32StruMess(String Type, int Number, int fmess32, String Source)
{
   // Ограничиваем длину Source до 7 символов
   String str;
   if (Source.length()>7) str=Source.substring(0,7);
   else str=Source;
   // Заполняем структуру
   strcpy(taskStruMess.Type, Type.c_str());                                
   if (Source==isOk) strcpy(taskStruMess.Source, SourceMessage.c_str());  
   else strcpy(taskStruMess.Source, str.c_str());                     
   taskStruMess.Number=Number;                                           
   sprintf(taskStruMess.fmess32, "%d", fmess32);                         
   strcpy(taskStruMess.smess32, EmptyMessage.c_str()); 
};    
// ****************************************************************************
// *     Сформировать сообщение c одним уточнением типа String (31 символ)   *
// ****************************************************************************
void TQueMessage::f32sStruMess(String Type, int Number, String fmess32)
{
   // Заполняем структуру: tStruMessage.Type
   strcpy(taskStruMess.Type, Type.c_str());   
   // tStruMessage.Source 
   strcpy(taskStruMess.Source, SourceMessage.c_str());  
   // Заполняем структуру: tStruMessage.Number 
   taskStruMess.Number=Number; 
   // Заполняем структуру: tStruMessage.fmess32 (ограничиваем длину до 31 символа)
   strcpy(taskStruMess.fmess32, fmess32.c_str()); 
   // Заполняем структуру: tStruMessage.smess32
   strcpy(taskStruMess.smess32, EmptyMessage.c_str()); 
};    
// ****************************************************************************
// *           Сформировать сообщение c двумя уточнениями целого типа         *
// ****************************************************************************
void TQueMessage::fs32StruMess(String Type, int Number, int fmess32, int smess32, String Source)
{
   // Ограничиваем длину Source до 7 символов
   String str;
   if (Source.length()>7) str=Source.substring(0,7);
   else str=Source;
   // Заполняем структуру
   strcpy(taskStruMess.Type, Type.c_str());                                
   if (Source==isOk) strcpy(taskStruMess.Source, SourceMessage.c_str());  
   else strcpy(taskStruMess.Source, str.c_str());                     
   taskStruMess.Number=Number;                                           
   sprintf(taskStruMess.fmess32, "%d", fmess32);                         
   sprintf(taskStruMess.smess32, "%d", smess32);
}; 
   
// Макрос: отправить сообщение из задачи или основного цикла
#define sendMess();                                                      \
   if (xQueueSend(tQueue,&taskStruMess,(TickType_t)0) != pdPASS)         \
   {                                                                     \
      sprintf(tBuffer,FailSend);                                         \ 
      inMess=String(tBuffer);                                            \
   }
// ****************************************************************************
// * 1 группа сообщений:            Отправить просто сообщение, без уточнений *
// ****************************************************************************
String TQueMessage::Send(String Type, int Number)
{
   // Инициируем успешное сообщение
   String inMess=isOk;
   // Если очередь создана, то отправляем сообщение в очередь
   if (tQueue!=0)
   {
      // Формируем сообщение для передачи в очередь
      simpStruMess(Type,Number); 
      // Отправляем сообщение
      sendMess();
   }
   // Отмечаем "Отправка сообщения: очередь структур не создана!" 
   else inMess=QueueNotSend;
   return inMess; 
}
String TQueMessage::SendISR(String Type,int Number) 
{
   // Инициируем пустое сообщение
   String inMess=isOk;
   // Если очередь создана, то отправляем сообщение в очередь
   if (tQueue!=0)
   {
      // Формируем сообщение для передачи в очередь
      simpStruMess(Type,Number); 
      // Сбрасываем признак переключения на более приоритетную задачу после прерывания 
      xHigherPriorityTaskWoken = pdFALSE;
      // Отправляем сообщение в структуре  
      if (xQueueSendFromISR(tQueue,&taskStruMess,&xHigherPriorityTaskWoken) != pdPASS)
      {
         // Если "Не удалось отправить структуру из прерывания!" 
         sprintf(tBuffer,FailSendInrupt); 
         inMess=String(tBuffer);
      }
   }
   // Отмечаем "Отправка сообщения: очередь структур не создана!" 
   else inMess=QueueNotSend;
   return inMess; 
}
// ****************************************************************************
// *  2 группа:           Отправить сообщение с первым уточнением целого типа *
// ****************************************************************************
String TQueMessage::Send(String Type, int Number, int fmess32, String Source)
{
   // Инициируем пустое сообщение
   String inMess=isOk;
   // Если очередь создана, то отправляем сообщение в очередь
   if (tQueue!=0)
   {
      // Формируем сообщение для передачи в очередь
      f32StruMess(Type, Number, fmess32, Source);    
      sendMess();
   }
   // Отмечаем "Отправка сообщения: очередь структур не создана!" 
   else inMess=QueueNotSend;
   return inMess; 
}
String TQueMessage::SendISR(String Type, int Number, int fmess32, String Source) 
{
   // Инициируем пустое сообщение
   String inMess=isOk;
   // Если очередь создана, то отправляем сообщение в очередь
   if (tQueue!=0)
   {
      // Формируем сообщение для передачи в очередь
      f32StruMess(Type, Number, fmess32, Source);    
      // Сбрасываем признак переключения на более приоритетную задачу после прерывания 
      xHigherPriorityTaskWoken = pdFALSE;
      // Отправляем сообщение в структуре 
      if (xQueueSendFromISR(tQueue,&taskStruMess,&xHigherPriorityTaskWoken) != pdPASS)
      {
         // Если "Не удалось отправить структуру из прерывания!" 
         sprintf(tBuffer,FailSendInrupt); 
         inMess=String(tBuffer);
      }
   }
   // Отмечаем "Отправка сообщения: очередь структур не создана!" 
   else inMess=QueueNotSend;
   return inMess; 
}
// ****************************************************************************
// *  3 группа:           Отправить сообщение с двумя уточнениями целого типа *
// ****************************************************************************
String TQueMessage::Send(String Type,int Number,int fmess32,int smess32,String Source)
{
   // Инициируем пустое сообщение
   String inMess=isOk;
   // Если очередь создана, то отправляем сообщение в очередь
   if (tQueue!=0)
   {
      // Формируем сообщение для передачи в очередь
      fs32StruMess(Type,Number,fmess32,smess32,Source);
      // Отправляем сообщение
      sendMess();
   }
   // Отмечаем "Отправка сообщения: очередь структур не создана!" 
   else inMess=QueueNotSend;
   return inMess; 
}
// ****************************************************************************
// *  4 группа:  Отправить сообщение c одним уточнением типа String (31 символ)
// ****************************************************************************
String TQueMessage::Send(String Type, int Number, String fmess32)
{
   // Инициируем пустое сообщение
   String inMess=isOk;
   // Если очередь создана, то отправляем сообщение в очередь
   if (tQueue!=0)
   {
      // Формируем сообщение для передачи в очередь
      f32sStruMess(Type, Number, fmess32);    
      sendMess();
   }
   // Отмечаем "Отправка сообщения: очередь структур не создана!" 
   else inMess=QueueNotSend;
   return inMess; 
}
// ****************************************************************************
// *          Извлечь информацию о текущем времени в отформатированном        *
// *                виде с помощью struct tm структуры данных:                *
// *                               https://cplusplus.com/reference/ctime/tm/  *
// ****************************************************************************
/*
 "%A, %B %d %Y %H:%M:%S" - это спецификаторы формата,  которые определяют,  как
 в struct tm timeinfo; будет отформатирован текст, а члены tm struct следующие:
  
  Тип элемента Значение                        Диапазон
  -----------------------------------------------------
  tm_sec  int  секунды после минуты            0-61*
  tm_min  int  минуты после часа               0-59
  tm_hour int  часы с полуночи                 0-23
  tm_mday int  день месяца                     1-31
  tm_mon  int  месяцы с января                 0-11
  tm_year int  годы с 1900
  tm_wday  —   количество дней с воскресенья   0-6
  tm_yday  —   количество дней с 1 января      0-365
  tm_isdst —   флаг перехода на летнее время 
  
  function strftime() - format time as string:
  https://cplusplus.com/reference/ctime/strftime/
*/
void TQueMessage::ExtractTime() 
{
   // Выбираем дату и время
   time_t rawtime;
   time(&rawtime);
   // Заполняем буфер
   strftime(dtime,20,"%Y-%m-%d,%H:%M:%S",localtime(&rawtime));
}
// ****************************************************************************
// *      Извлечь сообщение из массива по номеру и заполнить уточнениями      *
// ****************************************************************************
void TQueMessage::ExtractMess(String Source, int Number, String fmess32, String smess32) 
{
   sprintf(tMess,"Неопределенное сообщение примера очередей");
   for(int i=0; i<SizeMess; i++) 
   {
      if (amessAPP[i].num==Number)
      {
         // Выводим "простое сообщение, без уточнений"
         if (amessAPP[i].vmess==tvm_simpmes)
         { 
            sprintf(tMess,amessAPP[i].cmess); 
            break;                 
         }
         // Выводим "сообщение c одним уточнением целого типа"
         if (amessAPP[i].vmess==tvm_1intmes) 
         {
            sprintf(tMess,amessAPP[i].cmess,fmess32); 
            break;                 
         }
         // Выводим "сообщение c уточнением типа String"
         if (amessAPP[i].vmess==tvm_3strmes) 
         {
            sprintf(tMess,amessAPP[i].cmess,fmess32.c_str()); 
            break;                 
         }
         // Выводим "сообщение c одним уточнением целого типа"
         if (amessAPP[i].vmess==tvm_2intmes) 
         {
            sprintf(tMess,amessAPP[i].cmess,fmess32,smess32); 
            break;                  
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
// ****************************************************************************
// *            Определить, сколько сообщений накопилось в очереди            *
// *                            и их можно выгрузить                          *
// ****************************************************************************
int TQueMessage::How_many_wait()                 
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
int TQueMessage::How_many_free() 
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
char* TQueMessage::Receive(int t_MessFormat)
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
         // Если сообщение выбралось из очереди успешно, то собираем его в буфер
         if (xQueueReceive(tQueue,&receiveStruMess,(TickType_t )0)==pdPASS) 
         {        
            CollectMessage(t_MessFormat);
         }
         // Иначе отмечаем, что "Ошибка при приёме сообщения из очереди"  
         else sprintf(tBuffer,ErrorReceiving); 
      }
      // Отмечаем, что "Очередь пуста при приёме сообщения" 
      else if (nMess==0) sprintf(tBuffer,QueueEmptyReceive); 
      // "Не может быть!"
      else sprintf(tBuffer,"Не может быть: TQueMessage::Receive!");
   }
   // Отмечаем "Прием сообщения: очередь структур не создана"
   else sprintf(tBuffer,NoQueueReceive); 
   return tBuffer; 
}
// ****************************************************************************
// *          Выбрать сообщение из очереди и отправить на периферию           *
// ****************************************************************************
char* TQueMessage::Post(int t_MessFormat,char *prefix)
{
   Receive(t_MessFormat); 
   if (String(tBuffer)!=QueueEmptyReceive) (*atatchedF)(tBuffer,prefix); 
   return tBuffer; 
}
// ****************************************************************************
// *      Выбрать все сообщения разом из очереди и отправить на периферию     *
// ****************************************************************************
void TQueMessage::PostAll(int t_MessFormat,char *prefix)
{
   int iwait=How_many_wait();
   while(iwait>0)
   {
      Receive(t_MessFormat); 
      if (String(tBuffer)!=QueueEmptyReceive) (*atatchedF)(tBuffer,prefix); 
      iwait=How_many_wait();
   }
}

// ********************************************************* QueMessage.cpp ***
