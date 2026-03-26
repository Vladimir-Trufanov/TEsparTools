/** Arduino-Esp32-CAM                                     *** ---QueueChar.ino ***
 * 
 *                        -----Пример передачи сообщения из задачи и из прерывания с
 *                                                     приемом в основном цикле
 * 
 * v1.0.2, 26.03.2026                                 Автор:      Труфанов В.Е.
 * Copyright © 2024 tve                               Дата создания: 23.03.2026
**/


 // [1. Коды состояния ответа HTTP]
 // (https://developer.mozilla.org/ru/docs/Web/HTTP/Reference/Status)

 // [2. ESP32 WiFiMulti подключается к самой мощной сети Wi-Fi]
 // (https://microcontrollerslab.com/esp32-wifimulti-connect-to-the-strongest-wifi-network/)


// Определяем SSID собственной сети контроллера
// "Контроллер №1" - камера для съёмок на природе   ["DachaSad"]
// "ESP_75C391"    - ESP32-CAM, контроллер №3       ["NaDorogu"]
// "ESP_A7E119"    - ESP32-CAM, контроллер №6 
//#define soft_ap_ssid "DachaSad" 

// ---Подключаем файлы обеспечения передачи и приёма сообщений через очередь                //
//#include "ChipWiFi.h"     // з----аголовочный файл класса TQueMessage                         //
#include "DefWiFi.h"     // з----аголовочный файл класса TQueMessage                         //

void WiFiGotIP(WiFiEvent_t event, WiFiEventInfo_t info) 
{
  Serial.println("WiFi подключен");
  Serial.print("IP адресjr: "); Serial.println(IPAddress(info.got_ip.ip_info.ip.addr));
}

static unsigned long currentMillis;  // текущее время в миллисекундах 

void setup() 
{
  Serial.begin(115200);
  delay(1000);

  // Включаем прикладную обработку событий с WiFi через именные функции обратного вызова
  //WiFi.onEvent(WiFiGotIP, WiFiEvent_t::ARDUINO_EVENT_WIFI_STA_GOT_IP);
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

  Serial.println(ChipWiFi.Create());

  //wifiMulti.addAP("TP-Link_B394",  "18009217");
  //wifiMulti.addAP("tve-DESKTOP",   "Ue18-647");
  //wifiMulti.addAP("OPPO A9 2020",  "b277a4ee84e8");
  //wifiMulti.addAP("tve-MONOBLOCK", "Ue18-647");
  //wifiMulti.addAP("linksystve",    "X93K6KQ6WF");
  //wifiMulti.addAP("GoshaIMila",    "t1s2wde4bE");

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
