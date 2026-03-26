/** Arduino-Esp32-CAM                                *** WiFiMultiEvent.ino ***
 *   
 *                           Пример подключения к одной из локальных сетей WiFi 
 *          в соответствии с массивом учетных записей возможных локальных сетей
 *                     с учётом событий и создания собственной сети контроллера
 * 
 * v1.0.3, 26.03.2026                                 Автор:      Труфанов В.Е.
 * Copyright © 2024 tve                               Дата создания: 23.03.2026
**/

// Добавляем библиотеку для подключения к одной из локальных сетей WiFi в 
// соответствии с массивом учетных записей возможных локальных сетей и 
// создание собственной сети контроллера
// #include "ChipWiFi.h"

// ИЛИ из вне библиотеки подключаем и переопределяем параметры сетей 
// для ведения локальной и собственной сети WiFi
#include "DefWiFi.h"

void WiFiGotIP(WiFiEvent_t event, WiFiEventInfo_t info) 
{
  Serial.print("WiFi подключен. IP-адрес: "); Serial.println(IPAddress(info.got_ip.ip_info.ip.addr));
}

static unsigned long currentMillis;  // текущее время в миллисекундах 

void setup() 
{
  Serial.begin(115200);
  delay(1000);

  // Включаем прикладную обработку событий с WiFi через именные функции обратного вызова
  ChipWiFiEvent(WiFiGotIP, WiFiEvent_t::ARDUINO_EVENT_WIFI_STA_GOT_IP);
 
  // Включаем прикладную обработку событий с WiFi через безымянные лямбда-функции 
  WiFiEventId_t eventID = ChipWiFiEvent
  (
    [](WiFiEvent_t event, WiFiEventInfo_t info) 
    {
      Serial.print("Wi-Fi потерял связь. Причина: ");
      Serial.println(info.wifi_sta_disconnected.reason);
    },
    WiFiEvent_t::ARDUINO_EVENT_WIFI_STA_DISCONNECTED
  );
  // Строим объект класса (подключаемся к WiFi и создаём собственную сеть)
  ChipWiFi.Create();
  // Начинаем отсчет временных интервалов для сообщений в loop()
  currentMillis = millis();
}

void loop() 
{
  // Удерживаем подключение станции к WiFi 
  ChipWiFi.Keep();
  // Выводим контрольное сообщение после каждых 5 минут
  if ((millis()-currentMillis) > 5000) 
  { 
    Serial.println("+5000!");
    currentMillis = millis();
  }  
  // Делаем паузу для очистки мусора
  // (здесь две секунды, чтобы выводилась трассировка об отключении не часто,
  // в живых проектах лучше паузу делать меньше, например, 100 мс)
  delay(2000);
}
