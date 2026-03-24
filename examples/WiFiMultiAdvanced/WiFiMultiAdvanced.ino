
 // [1. Коды состояния ответа HTTP]
 // (https://developer.mozilla.org/ru/docs/Web/HTTP/Reference/Status)

 // [2. ESP32 WiFiMulti подключается к самой мощной сети Wi-Fi]
 // (https://microcontrollerslab.com/esp32-wifimulti-connect-to-the-strongest-wifi-network/)


// Определяем SSID собственной сети контроллера
// "Контроллер №1" - камера для съёмок на природе   ["DachaSad"]
// "ESP_75C391"    - ESP32-CAM, контроллер №3       ["NaDorogu"]
// "ESP_A7E119"    - ESP32-CAM, контроллер №6 
#define soft_ap_ssid "DachaSad" 


#include <WiFi.h>
#include <WiFiMulti.h>
#include <HTTPClient.h>

WiFiMulti wifiMulti;

// callback used to check Internet connectivity
bool testConnection() 
{
  HTTPClient http;
  http.begin("http://www.espressif.com");
  int httpCode = http.GET();
  // we expect to get a 301 because it will ask to use HTTPS instead of HTTP
  if (httpCode == HTTP_CODE_MOVED_PERMANENTLY) 
  {
    return true;
  }
  return false;
}


/*

* WiFi Events

typedef enum 
{
  [NetworkEvents.h](https://docs.espressif.com/projects/arduino-esp32/en/latest/api/network.html#network-events)
  [WiFi.h](https://espressif-docs.readthedocs-hosted.com/projects/arduino-esp32/en/latest/api/wifi.html)
  **Важно**: обработчики событий `WiFiEvent` вызываются из отдельной задачи (потока) FreeRTOS. Функции получают два параметра: WiFiEvent_t event и WiFiEventInfo_t info.
  WiFiEvent_t представляет тип события Wi-Fi. WiFiEventInfo_t содержит информацию о событии в системе событий сети на базе ESP32, это объединение значений типа system_event_info_t. 
  В этом объединении есть структура, которая содержит данные о событии. Например, в структуре, соответствующей событию SYSTEM_EVENT_STA_CONNECTED, есть члены: 
  ssid, ssid_len, bssid, channel и authmode. 
  --------------------------------------------------------------------------------------------------------------
  ARDUINO_EVENT_NONE = 0,  // 0
  ARDUINO_EVENT_ETH_START, // 1 - событие происходит во время настройки — когда запускается сервис Ethernet. В обработчике события можно, например:
                           // вывести сообщение «ETH Started»; настроить имя хоста Ethernet — оно должно быть установлено после запуска интерфейса, но перед DHCP.  
  ARDUINO_EVENT_ETH_STOP,  // 2 - означает, что работа с Ethernet прекращена. В обработчике можно вывести сообщение, например, Serial.println("ETH Stopped"). 
                           // Также можно установить, что работа с Ethernet прекращена - eth_connected = false. 
  ARDUINO_EVENT_ETH_CONNECTED, // 3 - возникает при установлении подключения по Ethernet. Событие запускается, когда физическое подключение (PHY) установлено.
                               // Это означает, что модуль Ethernet подключён к сети, и в системе изменился статус интерфейса — появился бит ESP_NETIF_CONNECTED_BIT. 
                               // ETHClass, который обеспечивает подключение к Ethernet, поддерживает две аппаратные конфигурации: а) RMII — внутренний MAC на ESP32 и 
                               // ESP32-P4. Эта конфигурация требует специальных пинов GPIO и обеспечивает меньшую задержку по сравнению с SPI; б) SPI — внешние 
                               // контроллеры MAC (например, W5500, DM9051, KSZ8851). Система поддерживает до трёх одновременных Ethernet-интерфейсов, каждый из которых 
                               // идентифицируется индексом (0–2) и имеет свой экземпляр esp_netif с настраиваемым приоритетом. Приоритет влияет на маршрутизацию, 
                               // когда несколько интерфейсов имеют подключение: интерфейсы с более высоким приоритетом становятся основным маршрутом. В коде для 
                               // обработки события ARDUINO_EVENT_ETH_CONNECTED обычно выполняется вывод сообщения — например, «ETH Connected».
  ARDUINO_EVENT_ETH_DISCONNECTED,  // 4 - означает отключение соединения PHY. Событие возникает, когда соединение PHY падает. Это может произойти, например: при отключении 
                                   // сетевого разъёма; при проблемах с подключением кабеля. В коде обработки можно вывести сообщение «ETH Disconnected» и 
                                   // установить значение eth_connected = false. 
  ARDUINO_EVENT_ETH_GOT_IP,  // 5 - событие возникает, когда DHCP назначает IPv4 и в системе меняется статус интерфейса: появляется бит ESP_NETIF_HAS_IP_BIT. В коде 
                             // обработки события можно вывести информацию о том, что Ethernet получил IP, и установить, что интерфейс подключён: "Получен IP-адрес". 
  ARDUINO_EVENT_ETH_LOST_IP, // 6 - означает потерю IPv4-адреса для интерфейса Ethernet. Событие возникает, когда адрес IPv4 становится недействительным. Это может 
                             // произойти, например: при разрыве соединения с сетью; при истечении срока аренды IP-адреса (например, после перезагрузки устройства).
                             // Событие не возникает сразу после отключения сети — вместо этого оно запускает таймер потери адреса. Если адрес IPv4 будет заново 
                             // получен до истечения времени таймера, событие не генерируется. В обработчике можно вывести в лог сообщение «ETH Lost IP»; сбросить
                             // флаг, который указывает на подключение к сети (eth_connected).
  ARDUINO_EVENT_ETH_GOT_IP6,  // 7 - cобытие возникает, когда DHCP назначает IPv6-адрес для интерфейса Ethernet. При этом в статусе интерфейса меняется бит 
                              // ESP_NETIF_HAS_LOCAL_IP6_BIT или ESP_NETIF_HAS_GLOBAL_IP6_BIT. 
#if SOC_WIFI_SUPPORTED || CONFIG_ESP_WIFI_REMOTE_ENABLED
  ARDUINO_EVENT_WIFI_OFF = 100,  //
  ARDUINO_EVENT_WIFI_READY,  // 8 - срабатывает, когда ESP32 готов к подключению к сети Wi-Fi. Оно может быть вызвано после завершения сканирования точки доступа 
                             // (ARDUINO_EVENT_WIFI_SCAN_DONE) или перед началом подключения (ARDUINO_EVENT_WIFI_STA_START). 
                             // В коде можно вывести сообщение «WiFi interface ready» в монитор
  ARDUINO_EVENT_WIFI_SCAN_DONE, // 9 - cобытие означает, что завершено сканирование точек доступа. Генерируется: если целевая точка доступа была успешно найдена;
                                // если все каналы были просканированы; если сканирование было принудительно прервано с помощью функции esp_wifi_scan_stop().
                                // В случае этого события можно вывести сообщение Serial.println("Completed scan for access points").
  ARDUINO_EVENT_WIFI_FTM_REPORT,  // 10 - срабатывает, когда драйвер WiFi получает результат процедуры FTM (Full-duplex Test Mode). FTM — это режим работы Wi-Fi, при 
                                  // котором устройство может принимать и передавать данные одновременно. В событии передаётся структура wifi_event_ftm_report_t, которая 
                                  // содержит данные, полученные в результате процедуры FTM. Некоторые поля структуры: status — статус отчета (например, FTM_STATUS_SUCCESS, 
                                  // FTM_STATUS_UNSUPPORTED и др.); dist_est — оценочное расстояние, полученное в результате FTM; rtt_est — время возврата данных.
                                  // Событие срабатывает после запуска сеанса FTM или при получении отчета о результате процедуры. 
                                  // Пример обработки: в коде может быть функция onFtmReport(arduino_event_t *event). В ней можно обрабатывать статус отчета, выводить данные 
                                  // и, если необходимо, освобождать указатель на отчет (например, free(report->ftm_report_data)). 
  ARDUINO_EVENT_WIFI_STA_START = 110,  // 11 - означает «начало работы станции (STA) ESP32». Режим STA — это режим, в котором контроллер не создаёт собственную сеть, 
                                       // а подключается к уже существующей сети Wi-Fi. Событие генерируется, когда запускается драйвер Wi-Fi. Это происходит, например, 
                                       // при вызове функции WiFi.begin(). Функция должна иметь параметры WiFiEvent_t event и WiFiEventInfo_t info.
В обработчике можно выполнять, например, выводить сообщение, указывающее на начало работы станции.
  ARDUINO_EVENT_WIFI_STA_STOP,  // 12 -  означает остановку режима станции (STA). Возникает, когда контроллер ESP32 прекращает работу в режиме STA. Событие сгенерируется, 
                                // когда контроллер ESP32 принудительно отключает режим STA с помощью функции esp_wifi_stop(). Также оно может быть сгенерировано, если режим 
                                // STA был принудительно прерван (например, из-за ошибки в работе драйвера). В данных события (event_data) содержатся биты, связанные с состоянием 
                                // режима STA: STA_STARTED, STA_CONNECTED, STA_GOT_IP и другие. Обработчик события ARDUINO_EVENT_WIFI_STA_STOP в коде на Arduino может выполнять 
                                // следующие действия: сбрасывать биты STA_STARTED, STA_CONNECTED, STA_GOT_IP, если они были установлены; отправлять сообщение об отключении STA 
                                // в прикладной цикл сообщений; останавливать и удалять таймер подключения, если он был. Если флаг STA_ENABLED установлен (то есть это не 
                                // принудительное отключение от WiFi), начинать процесс запуска режима STA заново. Иначе — выгружать из памяти всё, что было для работы STA.
  ARDUINO_EVENT_WIFI_STA_CONNECTED,  // 13 - сигнализирует о подключении станции (STA) к точке доступа. Это событие сгенерировано, когда контроллер (например, ESP32) успешно 
                                     // подключён к сети Wi-Fi в режиме STA. Событие генерируется, когда станция успешно отправляет запрос на подключение к точке доступа (AP). 
                                     // Точка доступа отвечает ответом на запрос, предоставляя (или отказывая) доступ к сети на уровне MAC. Данные события (event_data) содержат 
                                     // информацию о подключении: ssid — SSID подключённой точки доступа; ssid_len — длина SSID; bssid — MAC-адрес (BSSID) точки доступа, к 
                                     // которой подключена станция; channel — канал, используемый для соединения; authmode — режим аутентификации.
                                     // Важно: в момент события станция ещё не имеет IP-адреса и не может общаться с точкой доступа через протоколы TCP/IP. Событие можно обработать, 
                                     // зарегистрировав функцию-обработчик WiFi.onEvent(my_function, WIFI_EVENT_ID). В параметре WIFI_EVENT_ID нужно заменить имя или номер события. 
  ARDUINO_EVENT_WIFI_STA_DISCONNECTED,
  ARDUINO_EVENT_WIFI_STA_AUTHMODE_CHANGE,
  ARDUINO_EVENT_WIFI_STA_GOT_IP,
  ARDUINO_EVENT_WIFI_STA_GOT_IP6,
  ARDUINO_EVENT_WIFI_STA_LOST_IP,
  ARDUINO_EVENT_WIFI_AP_START = 130,
  ARDUINO_EVENT_WIFI_AP_STOP,
  ARDUINO_EVENT_WIFI_AP_STACONNECTED,
  ARDUINO_EVENT_WIFI_AP_STADISCONNECTED,
  ARDUINO_EVENT_WIFI_AP_STAIPASSIGNED,
  ARDUINO_EVENT_WIFI_AP_PROBEREQRECVED,
  ARDUINO_EVENT_WIFI_AP_GOT_IP6,
  ARDUINO_EVENT_WPS_ER_SUCCESS = 140,
  ARDUINO_EVENT_WPS_ER_FAILED,
  ARDUINO_EVENT_WPS_ER_TIMEOUT,
  ARDUINO_EVENT_WPS_ER_PIN,
  ARDUINO_EVENT_WPS_ER_PBC_OVERLAP,
  ARDUINO_EVENT_SC_SCAN_DONE = 150,
  ARDUINO_EVENT_SC_FOUND_CHANNEL,
  ARDUINO_EVENT_SC_GOT_SSID_PSWD,
  ARDUINO_EVENT_SC_SEND_ACK_DONE,
  ARDUINO_EVENT_PROV_INIT = 160,
  ARDUINO_EVENT_PROV_DEINIT,
  ARDUINO_EVENT_PROV_START,
  ARDUINO_EVENT_PROV_END,
  ARDUINO_EVENT_PROV_CRED_RECV,
  ARDUINO_EVENT_PROV_CRED_FAIL,
  ARDUINO_EVENT_PROV_CRED_SUCCESS,
#endif
  ARDUINO_EVENT_PPP_START = 200,
  ARDUINO_EVENT_PPP_STOP,
  ARDUINO_EVENT_PPP_CONNECTED,
  ARDUINO_EVENT_PPP_DISCONNECTED,
  ARDUINO_EVENT_PPP_GOT_IP,
  ARDUINO_EVENT_PPP_LOST_IP,
  ARDUINO_EVENT_PPP_GOT_IP6,
  ARDUINO_EVENT_MAX
} arduino_event_id_t;

5  ARDUINO_EVENT_WIFI_STA_DISCONNECTED         < ESP32 station disconnected from AP
6  ARDUINO_EVENT_WIFI_STA_AUTHMODE_CHANGE      < the auth mode of AP connected by ESP32 station changed
7  ARDUINO_EVENT_WIFI_STA_GOT_IP               < ESP32 station got IP from connected AP
8  ARDUINO_EVENT_WIFI_STA_LOST_IP              < ESP32 station lost IP and the IP is reset to 0
9  ARDUINO_EVENT_WPS_ER_SUCCESS       < ESP32 station wps succeeds in enrollee mode
10 ARDUINO_EVENT_WPS_ER_FAILED        < ESP32 station wps fails in enrollee mode
11 ARDUINO_EVENT_WPS_ER_TIMEOUT       < ESP32 station wps timeout in enrollee mode
12 ARDUINO_EVENT_WPS_ER_PIN           < ESP32 station wps pin code in enrollee mode
13 ARDUINO_EVENT_WIFI_AP_START                 < ESP32 soft-AP start
14 ARDUINO_EVENT_WIFI_AP_STOP                  < ESP32 soft-AP stop
15 ARDUINO_EVENT_WIFI_AP_STACONNECTED          < a station connected to ESP32 soft-AP
16 ARDUINO_EVENT_WIFI_AP_STADISCONNECTED       < a station disconnected from ESP32 soft-AP
17 ARDUINO_EVENT_WIFI_AP_STAIPASSIGNED         < ESP32 soft-AP assign an IP to a connected station
18 ARDUINO_EVENT_WIFI_AP_PROBEREQRECVED        < Receive probe request packet in soft-AP interface
19 ARDUINO_EVENT_WIFI_AP_GOT_IP6               < ESP32 ap interface v6IP addr is preferred
19 ARDUINO_EVENT_WIFI_STA_GOT_IP6              < ESP32 station interface v6IP addr is preferred
25 ARDUINO_EVENT_MAX

// WARNING: This function is called from a separate FreeRTOS task (thread)!
void WiFiEvent(WiFiEvent_t event) {
  Serial.printf("[WiFi-event] event: %d\n", event);

  switch (event) 
  {
    case ARDUINO_EVENT_WIFI_STA_CONNECTED:       
      Serial.println("Connected to access point"); 
      Serial.println("Всем привет!"); 
      break;
    case ARDUINO_EVENT_WIFI_STA_DISCONNECTED:   
      Serial.println("WiFi отключился!"); 
      Serial.println("Disconnected from WiFi access point"); 
      break;
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
     default:                                    break;
  }
}
*/

void WiFiGotIP(WiFiEvent_t event, WiFiEventInfo_t info) 
{
  Serial.println("WiFi connected");
  Serial.println("IP address станции: ");
  //Serial.println(WiFi.localIP());
  //Serial.println(IPAddress(info.got_ip.ip_info.ip.addr));
}

void WiFiDiscon(WiFiEvent_t event, WiFiEventInfo_t info) 
{
  Serial.println("Отключение");
  //Serial.println(info.disconnected.reason);
}

void Podkl()
{
  Serial.println("WiFi подключен");
  Serial.print("IP собственной сети: ");  Serial.print(WiFi.softAPIP()); Serial.print("  "); Serial.println(soft_ap_ssid);
  Serial.print("IP рабочей станции:  ");  Serial.print(WiFi.localIP());  Serial.print("  "); Serial.println(WiFi.SSID());
  Serial.print("RSSI = "); Serial.println(WiFi.RSSI());
}

void setup() 
{
  Serial.begin(115200);
  // delete old config
  //WiFi.disconnect(true);
  delay(1000);
  // Examples of different ways to register wifi events;
  // these handlers will be called from another thread.
  //WiFi.onEvent(WiFiEvent);
  WiFi.onEvent(WiFiGotIP,  WiFiEvent_t::ARDUINO_EVENT_WIFI_STA_GOT_IP);
  WiFi.onEvent(WiFiDiscon, WiFiEvent_t::ARDUINO_EVENT_WIFI_STA_DISCONNECTED);
  
  // Remove WiFi event
  //Serial.print("WiFi Event ID: ");
  //Serial.println(eventID);

  WiFi.mode(WIFI_MODE_APSTA);
  
  // Создаём собственную сеть
  WiFi.softAP(soft_ap_ssid,soft_ap_ssid);
  
  //wifiMulti.addAP("TP-Link_B394",  "18009217");
  //wifiMulti.addAP("tve-DESKTOP",   "Ue18-647");
  wifiMulti.addAP("OPPO A9 2020",  "b277a4ee84e8");
  wifiMulti.addAP("tve-MONOBLOCK", "Ue18-647");
  //wifiMulti.addAP("linksystve",    "X93K6KQ6WF");
  //wifiMulti.addAP("GoshaIMila",    "t1s2wde4bE");

  // These options can help when you need ANY kind of wifi connection to get a config file, report errors, etc.
  wifiMulti.setStrictMode(false);  // Default is true.  Library will disconnect and forget currently connected AP if it's not in the AP list.
  wifiMulti.setAllowOpenAP(true);  // Default is false.  True adds open APs to the AP list.
  wifiMulti.setConnectionTestCallbackFunc(testConnection);  // Attempts to connect to a remote webserver in case of captive portals.

  // Отключаем режим сна (modem sleep). 
  WiFi.setSleep(false);
  Serial.print("Подключение к WiFi ");
  if (wifiMulti.run()) Podkl();
  else Serial.println("В setup не подключились");
  /*
  while (wifiMulti.run() != WL_CONNECTED) 
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  */
}

void loop() 
{
  static bool isConnected;
  /*
  uint8_t WiFiStatus = wifiMulti.run();
  if (wifiMulti.run() == WL_CONNECTED) 
  {
    if (!isConnected) 
    {
      Serial.println("");
      Serial.println("WiFi connected");
      Serial.println("IP address: ");
      Serial.println(WiFi.localIP());
    }
    isConnected = true;
  } 
  else 
  {
    Serial.println("WiFi not connected!");
    isConnected = false;
    delay(5000);
  }
  */
  if (wifiMulti.run() == WL_CONNECTED) 
  {
    Podkl();
    Serial.println("В loop подключились");
    isConnected = true;
  } 
  else 
  {
    Serial.println("WiFi отключился!");
    isConnected = false;
  }
  delay(5000);
  Serial.println("=5000!");
}
