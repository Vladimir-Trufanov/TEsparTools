/** Arduino, Esp32 ******************************************* ChipWiFi.cpp ***
 * 
 *                          ------Обеспечить передачу и приём сообщений через очередь 
 *                                                   в задачах и из прерываниях
 * 
 * v1.0.1, 25.03.2026                                 Автор:      Труфанов В.Е.
 * Copyright © 2024 tve                               Дата создания: 23.03.2026
**/

#include "Arduino.h"

// Подключаем файлы обеспечения передачи и приёма сообщений через очередь 
#include "ChipWiFi.h"

WiFiMulti chipWiFi;

// ****************************************************************************
// *                    Объявить объект (конструктор класса)                  *
// ****************************************************************************
TChipWiFi::TChipWiFi()
{
}
// ****************************************************************************
// *    Проверить подключения к Интернету (функция обратного вызова класса)   *
// ****************************************************************************
bool testConnection() 
{
  HTTPClient http;
  http.begin("http://www.espressif.com");
  int httpCode = http.GET();
  // Ожидаем статус ответа, как 301, так как предполагается переадресация на HTTPS вместо HTTP
  if (httpCode == HTTP_CODE_MOVED_PERMANENTLY) 
  {
    return true;
  }
  return false;
}
// ****************************************************************************
// *   ------------------- Проверить подключения к Интернету (функция обратного вызова класса)   *
// ****************************************************************************
void WiFiEvent(WiFiEvent_t event) 
{
  Serial.printf("[WiFi-event: %d] ", event);

  switch (event) 
  {
    case ARDUINO_EVENT_WIFI_READY:               Serial.println("WiFi interface ready"); break;
    case ARDUINO_EVENT_WIFI_SCAN_DONE:           Serial.println("Completed scan for access points"); break;
    case ARDUINO_EVENT_WIFI_STA_START:           Serial.println("WiFi client started"); break;
    case ARDUINO_EVENT_WIFI_STA_STOP:            Serial.println("WiFi clients stopped"); break;
    case ARDUINO_EVENT_WIFI_STA_CONNECTED:       Serial.println("Connected to access point"); break;
    case ARDUINO_EVENT_WIFI_STA_DISCONNECTED:    Serial.println("Disconnected from WiFi access point"); break;
    case ARDUINO_EVENT_WIFI_STA_AUTHMODE_CHANGE: Serial.println("Authentication mode of access point has changed"); break;
    case ARDUINO_EVENT_WIFI_STA_GOT_IP:
      Serial.print("Obtained IP address: ");
      Serial.println(WiFi.localIP());
      break;
    case ARDUINO_EVENT_WIFI_STA_LOST_IP:        Serial.println("Lost IP address and IP address is reset to 0"); break;
    case ARDUINO_EVENT_WPS_ER_SUCCESS:          Serial.println("WiFi Protected Setup (WPS): succeeded in enrollee mode"); break;
    case ARDUINO_EVENT_WPS_ER_FAILED:           Serial.println("WiFi Protected Setup (WPS): failed in enrollee mode"); break;
    case ARDUINO_EVENT_WPS_ER_TIMEOUT:          Serial.println("WiFi Protected Setup (WPS): timeout in enrollee mode"); break;
    case ARDUINO_EVENT_WPS_ER_PIN:              Serial.println("WiFi Protected Setup (WPS): pin code in enrollee mode"); break;
    case ARDUINO_EVENT_WIFI_AP_START:           Serial.println("WiFi access point started"); break;
    case ARDUINO_EVENT_WIFI_AP_STOP:            Serial.println("WiFi access point  stopped"); break;
    case ARDUINO_EVENT_WIFI_AP_STACONNECTED:    Serial.println("Client connected"); break;
    case ARDUINO_EVENT_WIFI_AP_STADISCONNECTED: Serial.println("Client disconnected"); break;
    case ARDUINO_EVENT_WIFI_AP_STAIPASSIGNED:   Serial.println("Assigned IP address to client"); break;
    case ARDUINO_EVENT_WIFI_AP_PROBEREQRECVED:  Serial.println("Received probe request"); break;
    case ARDUINO_EVENT_WIFI_AP_GOT_IP6:         Serial.println("AP IPv6 is preferred"); break;
    case ARDUINO_EVENT_WIFI_STA_GOT_IP6:        Serial.println("STA IPv6 is preferred"); break;
    case ARDUINO_EVENT_ETH_GOT_IP6:             Serial.println("Ethernet IPv6 is preferred"); break;
    case ARDUINO_EVENT_ETH_START:               Serial.println("Ethernet started"); break;
    case ARDUINO_EVENT_ETH_STOP:                Serial.println("Ethernet stopped"); break;
    case ARDUINO_EVENT_ETH_CONNECTED:           Serial.println("Ethernet connected"); break;
    case ARDUINO_EVENT_ETH_DISCONNECTED:        Serial.println("Ethernet disconnected"); break;
    case ARDUINO_EVENT_ETH_GOT_IP:              Serial.println("Obtained IP address"); break;
    default:                                    Serial.println("Неопределённое событие"); break;
  }
}

// ****************************************************************************
// *                        Построить объект класса                           *
// ****************************************************************************
String TChipWiFi::Create()
{
  WiFi.mode(WIFI_MODE_APSTA);
  // Создаём собственную сеть
  WiFi.softAP(soft_ap_ssid,soft_ap_ssid);
  chipWiFi.addAP("OPPO A9 2020",  "b277a4ee84e8");
  chipWiFi.addAP("tve-MONOBLOCK", "Ue18-647");
  chipWiFi.addAP("linksystve",    "X93K6KQ6WF");

  // Отключаем строгий режим процесса подключения к сети WiFi
  // (если строгий режим включён setStrictMode(true), ESP32 прекращает попытки подключиться к дополнительным сетям,
  // как только успешно устанавливает соединение с первой доступной сетью из списка. Это обеспечивает более быстрое 
  // установление соединения, избегая ненужных попыток на другие сети; если строгий режим отключён setStrictMode(false), 
  // ESP32 может продолжать искать и пытаться подключиться к другим сетям в списке после установления соединения. 
  // Это полезно в сценариях, где нужно, чтобы устройство потенциально переключалось на лучшую сеть, если текущее 
  // соединение потеряно или неоптимально.
  chipWiFi.setStrictMode(false); 
  // Запрещаем подключения к открытым (незащищённым) точкам доступа
  chipWiFi.setAllowOpenAP(false);
  // Настраиваем вызов пользовательской функции (callback) для проверки достоверности соединения 
  // (синтаксис: chipWiFi.setConnectionTestCallbackFunc(callbackFunction). Аргумент callbackFunction — указатель на 
  // функцию типа ConnectionTestCB_t, который является typedef для std::function. Это пользовательская функция, которая 
  // возвращает значение bool. Она вызывается после подключения к AP для проверки достоверности соединения (например, 
  // доступности интернета). Функция должна возвращать true, если соединение соответствует критериям, или false, 
  // чтобы вызвать переход к другой AP. Метод не возвращает значение (тип возврата — void). Он настраивает объект WiFiMulti 
  // на использование предоставленной функции-обратного вызова во время попыток подключения)
  chipWiFi.setConnectionTestCallbackFunc(testConnection);  // Attempts to connect to a remote webserver in case of captive portals.
  // Отключаем режим сна (modem sleep). 
  WiFi.setSleep(false);

  // Включаем штатную обработку всех событий с WiFi
  WiFi.onEvent(WiFiEvent);




  // Подключаем станцию к WiFi  
  Keep();


   // Инициируем успешное сообщение
   String inMess="tiыыsOk";
   //tQueue = xQueueCreate(QueueSize, sizeof(struct tStruMess));
   // Возвращаем ошибку "Очередь не была создана и не может использоваться" 
   //if (tQueue==NULL) inMess=tQueueNotCreate; 
   return inMess;
};
// ****************************************************************************
// *                 Удерживать подключение станции к WiFi                    *
// ****************************************************************************
void TChipWiFi::Keep()
{
  if (chipWiFi.run() == WL_CONNECTED) 
  {
    // Если первый раз подключились после разрыва, то выводим сообщения
    if (!isConnected) 
    {
      Serial.println("WiFi подключен!");
      Serial.print("IP собственной сети: ");  Serial.print(WiFi.softAPIP()); Serial.print("  "); Serial.println(soft_ap_ssid);
      Serial.print("IP рабочей станции:  ");  Serial.print(WiFi.localIP());  Serial.print("  "); Serial.println(WiFi.SSID());
      Serial.print("RSSI = "); Serial.println(WiFi.RSSI());
      isConnected = true;
    }
  } 
  else 
  {
    // Выводим сообщение об отключении 
    Serial.println("WiFi отключился!");
    isConnected = false;
  }
}

// ************************************************************ ChipWiFi.cpp ***
