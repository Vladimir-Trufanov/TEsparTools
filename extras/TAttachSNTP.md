## [TAttachSNTP -  Подключиться к пулу серверов точного времени и синхронизировать время контроллера по протоколу SNTP](#) v1.0.2, 02.01.2025 

Класс ***TAttachSNTP*** предназначен для подключения часов контроллера ESP32 к заданному пулу серверов точного времени по протоколу SNTP и обеспечения синхронизации даты и время контроллера с серверами. 

Некоторые пулы серверов точного времени:

- ***ru.pool.ntp.org***, Российская Федерация;
- ***ntp.msk-ix.ru*** , Москва, Столица Российской Федерации;
- ***pool.ntp.org***, основной мировой пул точного времени;
- ***time.google.com***, сервера Google: Google Public NTP [AS15169];
- ***time.windows.com***, сервера Microsoft NTP server [AS8075].


Некоторые временные зоны (часовые пояса)
[https://github.com/nayarsystems/posix_tz_db/blob/master/zones.csv](https://github.com/nayarsystems/posix_tz_db/blob/master/zones.csv):

```
   1 Africa/Abidjan     GMT0
   2 Africa/Accra       GMT0
  13 Africa/Cairo       EET-2EEST,M4.5.5/0,M10.5.4/24
  14 Africa/Casablanca  <+01>-1
  54 America/Anchorage  AKST9AKDT,M3.2.0,M11.1.0
  85 America/Caracas    <-04>4
 154 America/New_York   EST5EDT,M3.2.0,M11.1.0
 350 Europe/Moscow      MSK-3
 430 Etc/GMT-2          <+02>-2
 431 Etc/GMT-3          <+03>-3
```
ESP32 использует два аппаратных таймера для отслеживания системного времени:
 
- таймер RTC: позволяет отслеживать время в различных режимах сна, а также сохранять время при любых сбросах (за исключением сброса при включении питания, который сбрасывает таймер RTC). Отклонение частоты зависит от источников тактового сигнала таймера RTC и влияет на точность только в режимах сна, в которых время будет измеряться с точностью до 6,6667 мкс;

- таймер с высоким разрешением: этот таймер недоступен в спящих режимах и не сохраняется после перезагрузки, но обладает большей точностью. Таймер использует источник тактовых импульсов APB_CLK (обычно 80 МГц), отклонение частоты которого составляет менее ±10 частей на миллион. Время измеряется с точностью до 1 мкс.


Системное время можно отслеживать с помощью одного или обоих аппаратных таймеров в зависимости от целей приложения и требований к точности системного времени (при умалчиваемом режиме подключены оба таймера, это гарантирует, что после синхронизации с сервером SNTP время будет отсчитываться с помощью обоих таймеров). 

Когда контроллер ESP32 находится в режиме глубокого сна, для отслеживания времени используется встроенный таймер RTC, а таймер FRC1 с высоким разрешением включается, когда плата переходит в активный режим. 

### Методы класса TAttachSNTP

- ***Определить объект (конструктор класса)***. 

При определении в приложении переменной для объекта класса (то есть при вызове конструктора класса) можно указать пул серверов точного времени, по умолчанию назначается ***ntp.msk-ix.ru***.

Формат:

```
 TAttachSNTP(const char *server="ntp.msk-ix.ru");

```

- ***Построить объект синхронизации времени***

При вызове данного метода создаётся объект синхронизации времени. Методу передаются (могут быть переданы) два параметра: интервал синхронизации времени контроллера с серверами точного времени в минутах ***smin*** и указатель на последовательность символов с завершающим нулем ***zone*** - указывающих на временную зону (часовой пояс).
 
По умолчанию интервал синхронизации устанавливается равным 60 минутам, а временная зона "московское время" - "MSK-3".

Формат:
```
void Create(int smin=60, const char *zone="MSK-3");
```
При вызове метода ***Create*** подключается сервер точного времени из указанного пула серверов, часы контроллера устанавливаются в точное значение для заданного часового пояса и, в дальнейшем, всегда будут синхронизироваться через указанный интервал с сервером точного времени.

- ***Извлечь информацию о текущей дате и времени*** 

Данный метод возвращает строку с текущей датой и временем по местному времени в типизированном виде: "***2024-12-28 13:29:07***".

Формат:
```       
String strTime(); 
```
- ***Извлечь информацию о дате и времени в указанной временной зоне***

Этот метод выводит строку с текущей датой и временем в указанной (переданной через параметр) временной зоне (часовом поясе). Например, в Токио - ***strLocalTime("JST-9")*** или в Ульяновске - ***strLocalTime("<+04>-4")***. 

Формат:
```       
String strLocalTime(const char* value="MSK-3");
```

- ***Узнать подключенный пул серверов точного времени***

Формат:
```       
String getservername();
```

- ***Узнать установленную временную зону (часовой пояс)***

Формат:
```       
String gettimezone();
```

- ***Узнать число минут (интервал) синхронизации***

Формат:
```       
String getsynchromin();
```

