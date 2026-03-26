# [TEsparTools, v3.4.1](https://github.com/Vladimir-Trufanov/TEsparTools) 26.03.2026 

Библиотека прикладных классов и функций в IDE Arduino для ESP. Как правило, модули библиотеки опираются на операционную систему ***FreeRTOS***.

---

### TAttachSNTP -  [Подключиться к пулу серверов точного времени и синхронизировать время контроллера по протоколу SNTP](extras/TAttachSNTP.md) v1.0.2, 02.01.2025 

### TChipWiFi -  [Обеспечить поиск и подключение к одной из сетей WiFi в соответствии с массивом учетных записей возможных локальных сетей, обработать события подключения, создать собственную сеть контроллера](#) v1.0.5, 26.03.2026 

### TQue - [Обеспечить передачу и приём сообщений "как есть", максимум 1023 символа, плюс завершающий ноль, через очередь в задачах и из прерываний](extras/TQue.md) - v1.0.0, 23.12.2024 

### TQueMessage - [Обеспечить передачу и приём сообщений через очередь в задачах и из прерываний](extras/TQueMessage.md) - v3.3.0, 09.03.2025

---

### library.properties

```
name=TEsparTools
version=3.4.1
author=Труфанов Владимир Евгеньевич, tve@karelia.ru, <tve58@inbox.ru>
maintainer=Trufanov Vladimir https://doortry.ru/kroshki-opyta/
sentence=Library of application classes and functions ESP32-FreeRTOS on IDE Arduino 1.8.19+, 2.3.8
paragraph=Библиотека прикладных классов и функций в IDE Arduino 1.8.19+, 2.3.8, Espressif Systems 3.3.5+
category=Device Control
url=https://github.com/Vladimir-Trufanov/TEsparTools
architectures=*
```
### Обновления

#### v3.4.1, 26.03.2026

Добавлен класс TChipWiFi - Обеспечить поиск и подключение к одной из локальных сетей WiFi в соответствии с массивом учетных записей возможных локальных сетей  с учётом событий и создание собственной сети контроллера

#### v3.3.0, 09.03.2025

Добавлен метод для отправки сообщения c одним уточнением типа String (31 символ) в класс TQueMessage - обеспечить передачу и приём сообщений через очередь в задачах и из прерываний:

```
// 4 группа: Отправить сообщение c одним уточнением типа String (31 символ)
String Send(String Type, int Number, String fmess32);
```

#### v3.2.9, 02.01.2025

Добавлен класс TAttachSNTP - Подключиться к пулу серверов точного времени и синхронизировать время контроллера по протоколу SNTP.

#### v3.2.8, 27.12.2024 

Добавлен контроль размерности ввода типа сообщения и источника сообщения.

### Библиография

#### 1. [Writing a Library for Arduino](https://docs.arduino.cc/learn/contributions/arduino-creating-library-guide/)

#### 2. [Library specification](https://arduino.github.io/arduino-cli/1.1/library-specification/)

#### 3. [Семантическое Версионирование 2.0.0](https://semver.org/lang/ru/)

#### 4. [Пишем свою библиотеку](https://alexgyver.ru/lessons/library-writing/)

