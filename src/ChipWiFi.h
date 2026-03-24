/** Arduino, ESP32, C/C++ ************************************** ChipWiFi.h ***
 * 
 *                        Обеспечить сканирование и ведение локальной сети WiFi
 *                                               и собственной сети контроллера 
 *                                                     
 * v1.0.1, 20.03.2026                                 Автор:      Труфанов В.Е.
 * Copyright © 2026 tve                               Дата создания: 18.03.2026
**/

#pragma once     

#include "WiFi.h"

// Определяем SSID собственной сети контроллера
// "DachaSad"    - камера для съёмок на природе
// "ESP_75C391"  - ESP32-CAM, контроллер №3
// "ESP_A7E119"  - ESP32-CAM, контроллер №6 ["NaDorogu"]
#define soft_ap_ssid "DachaSad" 

// Указываем учетные данные сетей WiFi, которые может использовать контроллер
const char* sarr[] =
{
  "TP-Link_B394",  "18009217",
  "tve-DESKTOP",   "Ue18-647",
  "OPPO A9 2020",  "b277a4ee84e8",
  "tve-MONOBLOCK", "Ue18-647",
  "linksystve",    "X93K6KQ6WF"
};

// Резервируем параметры для выбираемой сети
char essid[16] = {0};       // идентификатор беспроводной сети
char epassword[16] = {0};   // пароль
int eRSSI = -999;           // уровень принимаемого сигнала (дБм - децибел на милливатт) 

// Просканировать, показать сети WiFi в диапазоне и выбрать с большим RSSI 
bool ViewWiFi(); 
// Просканировать сети WiFi в диапазоне по списку и выбрать подходящую для подключения 
bool ScanWiFi(); 
// Проверить соответствие заданному ssid одной из сетей WiFi и выбрать сеть для подключения 
bool findWiFi(const char* ssid, int iRSSI);
// Инициировать работу контроллера, как станции WiFi и с собственной сетью
void InitWiFi(const char* ssid, const char* password);

// ****************************************************************************
// *     Проверить соответствие заданному ssid одной из сетей WiFi, которые   *
// *      может использовать контроллер и выбрать эту сеть для подключения    *
// *        [essid,epassword], если уровень сигнала превышает ранее           *
// *                    зафиксированный уровень другой сети                   *
// ****************************************************************************
bool findWiFi(const char* ssid, int iRSSI) 
{
  bool result=false;
  for (int i = 0; i < 9; i=i+2) 
  {
    //Serial.print("Смотри: "); Serial.print(iRSSI); Serial.print(" "); Serial.print(sarr[i]); Serial.print(" => ");  Serial.println(sarr[i+1]);
    int res = strcmp(sarr[i], ssid);
    if (res == 0) 
    {
      //Serial.print("Совпал: "); Serial.print(iRSSI); Serial.print(" "); Serial.print(sarr[i]); Serial.print(" => ");  Serial.println(sarr[i+1]);
      if (((strlen(sarr[i]))<16) || ((strlen(sarr[i]))<16))
      {
        //Serial.print("Годен:  "); Serial.print(iRSSI); Serial.print(" "); Serial.print(sarr[i]); Serial.print(" => ");  Serial.println(sarr[i+1]);
        //Serial.print("Было:   "); Serial.print(eRSSI); Serial.print(" "); Serial.print(essid);   Serial.print(" => ");  Serial.println(epassword);
        if (iRSSI>eRSSI)
        {
          // essid[16] = {0};
          // Serial.print(essid); Serial.print(" 0=> ");  Serial.println(epassword);
          strncpy(essid, sarr[i], sizeof(essid) - 1); 
          // epassword[16] = {0};
          strncpy(epassword, sarr[i+1], sizeof(epassword) - 1); 
          eRSSI=iRSSI;
          //Serial.print("Стало:  "); Serial.print(eRSSI); Serial.print(" "); Serial.print(essid);   Serial.print(" => ");  Serial.println(epassword);
          result=true;
          break;
        }
      }
    }
  }
  return result; 
}
// ****************************************************************************
// *              Просканировать сети WiFi в диапазоне по списку и            *
// *                     выбрать подходящую для подключения                   *
// ****************************************************************************
void SerialDefis()
{
  Serial.println("--------------------------------------------------------------");
}
bool ScanWiFi() 
{
  // Отмечаем, что подходящая сеть ещё не найдена
  bool isnetwifi=false;
  //Serial.println("Начато сканирование сетей WiFi");
  int n = WiFi.scanNetworks(); // получили количество найденных сетей
  //Serial.println("Сканирование завершено");
  if (n == 0) 
  {
    Serial.println("Сетей не найдено");
  } 
  else 
  {
    //Serial.print(n); Serial.println(": столько сетей найдено");
    Serial.println("№  | SSID                             | RSSI | CH | Encryption");
    for (int i = 0; i < n; ++i) 
    {
      // Выводим данные текущей сети
      Serial.printf("%2d", i + 1);
      Serial.print(" | ");
      Serial.printf("%-32.32s", WiFi.SSID(i).c_str());
      Serial.print(" | ");
      Serial.printf("%4ld", WiFi.RSSI(i));
      Serial.print(" | ");
      Serial.printf("%2ld", WiFi.channel(i));
      Serial.print(" | ");
      switch (WiFi.encryptionType(i)) 
      {
        case WIFI_AUTH_OPEN:            Serial.print("open"); break;
        case WIFI_AUTH_WEP:             Serial.print("WEP"); break;
        case WIFI_AUTH_WPA_PSK:         Serial.print("WPA"); break;
        case WIFI_AUTH_WPA2_PSK:        Serial.print("WPA2"); break;
        case WIFI_AUTH_WPA_WPA2_PSK:    Serial.print("WPA+WPA2"); break;
        case WIFI_AUTH_WPA2_ENTERPRISE: Serial.print("WPA2-EAP"); break;
        case WIFI_AUTH_WPA3_PSK:        Serial.print("WPA3"); break;
        case WIFI_AUTH_WPA2_WPA3_PSK:   Serial.print("WPA2+WPA3"); break;
        case WIFI_AUTH_WAPI_PSK:        Serial.print("WAPI"); break;
        default:                        Serial.print("unknown");
      }
      Serial.println();
      // Отмечаем, если выбрана подходящая сеть
      bool isWiFi=findWiFi(WiFi.SSID(i).c_str(), WiFi.RSSI(i));
      if (isWiFi) 
      {
        Serial.print("Можно подключиться: ");
        Serial.print(essid); Serial.print(" => "); Serial.print(epassword); Serial.print(" = ");  Serial.println(eRSSI);
        isnetwifi=true;
      }
      delay(10);
    }
  }
  // Удаляем результат сканирования, чтобы освободить память
  WiFi.scanDelete();
  SerialDefis();
  return isnetwifi;
}
// ****************************************************************************
// *  Просканировать, показать сети WiFi в диапазоне и выбрать с большим RSSI *
// ****************************************************************************
bool ViewWiFi() 
{
  bool iswifiview;
  // Переводим Wi-Fi в режим станции и отключаемся от точки доступа, если она была подключена
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  // Ждем немного перед сканированием
  delay(1000);
  // Для ранних версий ESP_IDF не выделяем отдельных диапазонов
  SerialDefis();
  Serial.println("Отсканированные сети Wi-Fi:");
  SerialDefis();
  // Для поздних версий ESP_IDF указываем автоматический выбор диапазона 
  #if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(5, 4, 2)
    WiFi.setBandMode(WIFI_BAND_MODE_AUTO);
  #endif
  iswifiview=ScanWiFi();
  // Для поздних версий ESP_IDF с поддержкой 5G вначале выбираем диапазон 2.4 Ghz 
  #if CONFIG_SOC_WIFI_SUPPORT_5G
    delay(1000);
    SerialDefis();
    Serial.println("Отсканированные сети в диапазоне WiFi 2,4 ГГц:");
    SerialDefis();
    WiFi.setBandMode(WIFI_BAND_MODE_2G_ONLY);
    iswifiview=ScanWiFi();
    delay(1000);
    // Затем выбираем диапазон 5 Ghz 
    SerialDefis();
    Serial.println("Отсканированные сети в диапазоне WiFi 5 ГГц");
    SerialDefis();
    WiFi.setBandMode(WIFI_BAND_MODE_5G_ONLY);
    iswifiview=ScanWiFi();
  #endif
  //if (iswifiview) Serial.println("Точно сеть найдена!");
  //else Serial.println("Точно сеть НЕ найдена!");
  return iswifiview;
}
// ****************************************************************************
// *   Инициировать работу контроллера, как станции WiFi и с собственной сетью  *
// ****************************************************************************
void InitWiFi(const char* ssid, const char* password)
{
  // Переключаем контроллер в режим работы с собственной сетью и как станции

  WiFi.mode(WIFI_MODE_APSTA);
  // Создаём собственную сеть
  WiFi.softAP(soft_ap_ssid,soft_ap_ssid);
  // Подключаемся к WiFi
  WiFi.begin(ssid,password);
  // Отключаем режим сна (modem sleep). 
  WiFi.setSleep(false);
  Serial.print("Подключение к WiFi ");
  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi подключен");
  Serial.print("IP собственной сети: ");  Serial.print(WiFi.softAPIP()); Serial.print("  "); Serial.println(soft_ap_ssid);
  Serial.print("IP рабочей станции:  ");  Serial.print(WiFi.localIP());  Serial.print("  "); Serial.println(ssid);
}

// ************************************************************* ChipWiFi.h ***
