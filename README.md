# TEsparTools

Библиотека прикладных классов и функций в IDE Arduino для ESP. Как правило, модули библиотеки опираются на операционную систему ***FreeRTOS***.

##### На [2024-12-11]() в состав библиотеки входит только один класс - TQueMessage. 

## TEsparTools, v3.2.1

---

### [TQueMessage - обеспечить передачу и приём сообщений через очередь в задачах и из прерываний](#tquemessage---%D0%BE%D0%B1%D0%B5%D1%81%D0%BF%D0%B5%D1%87%D0%B8%D1%82%D1%8C-%D0%BF%D0%B5%D1%80%D0%B5%D0%B4%D0%B0%D1%87%D1%83-%D0%B8-%D0%BF%D1%80%D0%B8%D1%91%D0%BC-%D1%81%D0%BE%D0%BE%D0%B1%D1%89%D0%B5%D0%BD%D0%B8%D0%B9-%D1%87%D0%B5%D1%80%D0%B5%D0%B7-%D0%BE%D1%87%D0%B5%D1%80%D0%B5%D0%B4%D1%8C-%D0%B2-%D0%B7%D0%B0%D0%B4%D0%B0%D1%87%D0%B0%D1%85-%D0%B8-%D0%B8%D0%B7-%D0%BF%D1%80%D0%B5%D1%80%D1%8B%D0%B2%D0%B0%D0%BD%D0%B8%D0%B9) v3.2.1

---

### TQueMessage - обеспечить передачу и приём сообщений через очередь в задачах и из прерываний

Класс ***TQueMessage*** предназначен для организации единообразной передачи сообщений в последовательный порт или на другую периферию. 

В классе определены 5 типов сообщений:

```
#define tmt_NOTICE  "NOTICE"     // информационное сообщение приложения 
#define tmt_TRACE   "TRACE"      // трассировочное сообщение при отладке
#define tmt_WARNING "WARNING"    // предупреждение, позволяющие работать задаче дальше 
#define tmt_ERROR   "ERROR"      // ошибка, не дающие возможность правильно выполнить задачу
#define tmt_FATAL   "FATAL"      // ошибка, вызывающие перезагрузку контроллера
``` 

Существуют три формата вывода сообщений в приложениях: краткий, полный, без даты и времени.

```
typedef enum {
   tfm_BRIEF,   // 0 Краткий             - WARNING-ISR[2]
   tfm_NOTIME,  // 1 Без даты и времени  - WARNING-ISR[2] Управление передаётся планировщику
   tfm_FULL,    // 2 Полный              - 2024-11-29,19:36:18 WARNING-ISR[2] Управление передаётся планировщику
} tFMess;
```

В полном сообщении указывается дата и время извлечения сообщения из очереди, тип сообщения, источник сообщения, номер сообщения источника, текст сообщения.

> Источник сообщения - это псевдоним приложения (модуля, библиотеки, любого объекта выполнения кода), которое отправляет сообщение.  Источник сообщения **tmk_APP** определяется автором продукта через директиву **#define**. 

В примере ***QueueHandlMulti.ino*** к классу ***TQueMessage***   источнику сообщения присваивается псевдоним ***"QHM"***:

```
// Определяем источник сообщений  
#define tmk_APP  "QHM"  // пример по обработке очередей
```

Некоторые примеры полных сообщений:
```
2024-11-29,19:36:18 WARNING-ISR[2]    Управление передаётся планировщику   
2024-11-29,19:38:45 ERROR-EUE[0]      Очередь не была создана и не может использоваться  
2024-11-30,08:11:54 NOTICE-KVIZZY[2]  Передано 124 сообщение из задачи  
```


---

#### [Внешняя функция пeредачи сообщения на периферию](#%D0%B2%D0%BD%D0%B5%D1%88%D0%BD%D1%8F%D1%8F-%D1%84%D1%83%D0%BD%D0%BA%D1%86%D0%B8%D1%8F-%D0%BF%D0%B5%D1%80%D0%B5%D0%B4%D0%B0%D1%87%D0%B8-%D1%81%D0%BE%D0%BE%D0%B1%D1%89%D0%B5%D0%BD%D0%B8%D1%8F-%D0%BD%D0%B0-%D0%BF%D0%B5%D1%80%D0%B8%D1%84%D0%B5%D1%80%D0%B8%D1%8E)

#### [Порядок фoрмирования списка сообщений приложения](#%D0%BF%D0%BE%D1%80%D1%8F%D0%B4%D0%BE%D0%BA-%D1%84%D0%BE%D1%80%D0%BC%D0%B8%D1%80%D0%BE%D0%B2%D0%B0%D0%BD%D0%B8%D1%8F-%D1%81%D0%BF%D0%B8%D1%81%D0%BA%D0%B0-%D1%81%D0%BE%D0%BE%D0%B1%D1%89%D0%B5%D0%BD%D0%B8%D0%B9-%D0%BF%D1%80%D0%B8%D0%BB%D0%BE%D0%B6%D0%B5%D0%BD%D0%B8%D1%8F)

#### [Мeтоды класса TQueMessage](#%D0%BC%D0%B5%D1%82%D0%BE%D0%B4%D1%8B-%D0%BA%D0%BB%D0%B0%D1%81%D1%81%D0%B0-tquemessage)


#### [Подключение файлов и создание очереди сообщений](#)

---

#### Внешняя функция передачи сообщения на периферию

Направление передачи информации определяется во внешней функции ***transmess***, которая может быть изменена пользователем. 

Функции передаются указатели на два массива символов с завершающим нулём: ***mess*** - собственно сообщение, которое проходит через очередь, и ***prefix*** - префикс сообщения (по умолчанию отсутствует).

Префикс сообщения, это краткий фрагмент текста, который выводится перед сообщением. Назначение или смысл префикса определяются разработчиком приложения.

```
// Передатчик сообщения на периферию с возможным префиксом:
// static char str[] = "Hello: ";
// queMessa.Post(queMessa.Receive(MessFormat),str);

inline void transmess(char *mess, char *prefix="") 
{
   // Выводим массивы символов с 0-вым окончанием
   Serial.print(prefix);  // передали префикс (по умолчанию отсутствует)
   Serial.println(mess);  // передали сообщение
}
```
#### Порядок формирования списка сообщений приложения

Для того, чтобы использовать класс TQueMessage для передачи сообщений, необходимо их объявить через перечисление и подготовить case - конструкцию для переноса сообщений в буфер подготовить 



#### Методы класса TQueMessage

- ***Построить объект (конструктор класса)***. 

При определении в приложении переменной для объекта класса (то есть при вызове конструктора класса) можно указать максимальное количество сообщений, которое может быть помещено в очередь, по умолчанию 4 сообщения.

Формат:

```
TQueMessage(int iQueueSize=4);
```

- ***Создать очередь***

При вызове данного метода резервируется пространство в оперативной памяти для размещения сообщений очереди.

Формат:
```
String Create();
```
Если функции не удалось создать очередь, то она вернет строку ***tQueueNotCreate*** =  "***Очередь не была создана и не может использоваться***".

-  ***Отправить сообщение с первым уточнением целого типа***

Указанным методом выполняется отправка сообщения в очередь с использованием дополнительного целочисленного параметра. Здесь используются два варианта: ***Send*** - для выделенных задач в скетче, основного цикла и блока настройки setup; ***SendISR*** - для отправки сообщения из прерывания.

Формат:

```
String Send(String Type,String Source,int Number,int fmess32); 
String SendISR(String Type,String Source,int Number,int fmess32); 
```




- ***Прикрепить внешнюю функцию по параметрам***

   void attachFunction(void (*function)(char *mess, char *prefix));






#### Подключение файлов и создание очереди сообщений



В состав класса 

// Подключаем файлы обеспечения передачи и приёма сообщений через очередь 
#include "QueMessage.h"     // заголовочный файл класса TQueMessage 
#include "CommonMessage.h"  // общий реестр сообщений
#include "QHM_Message.h"    // сообщения примера по обработке очередей




1

1

1

1

1

1

1

1



















#### library.properties

```
name=TEsparTools
version=3.2.1
author=Труфанов Владимир Евгеньевич, tve@karelia.ru, <tve58@inbox.ru>
maintainer=Trufanov Vladimir https://doortry.ru/kroshki-opyta/
sentence=Library of application classes and functions ESP32-FreeRTOS on IDE Arduino 1.8.19
paragraph=Библиотека прикладных классов и функций в IDE Arduino 1.8.19, Espressif Systems 3.0.5+
category=Device Control
url=https://github.com/Vladimir-Trufanov/TEsparTools
architectures=*
```


### Библиография

#### 1. [Writing a Library for Arduino](https://docs.arduino.cc/learn/contributions/arduino-creating-library-guide/)

#### 2. [Library specification](https://arduino.github.io/arduino-cli/1.1/library-specification/)

#### 3. [Семантическое Версионирование 2.0.0](https://semver.org/lang/ru/)

###### [в начало](#tespartools)