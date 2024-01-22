//-------------------------------- Libraries -----------------------------------
#include <SD.h>                     // подключаем библиотеку для работы sd-card reader
#include <charMap.h>                // библиотека для отрисовки символов на дисплее
#include <SPI.h>                    // подключаем библиотеку для работы шины SPI
#include <LiquidCrystal_I2C.h>      // подключаем библиотеку для работы с дисплеем https://www.arduino.cc/reference/en/libraries/liquidcrystal-i2c/ 
#include <Wire.h>                   // подключаем библиотеку "Wire" - Стандартная библиотека
#include <GyverMAX6675_SPI.h>       // подключаем библиотеку для работы c термопарой https://github.com/GyverLibs/GyverMAX6675
#include <GyverGFX.h>               // подключаем библиотеку для работы графики дисплея https://github.com/GyverLibs/GyverGFX
#include <microDS3231.h>            // подключаем библиотеку для работы с RTC модулем https://github.com/GyverLibs/microDS3231
//#include <iarduino_RTC.h>
//-------------------------------- Libraries -----------------------------------

//-------------------------------- ОБЪЯВЛЕНИЕ ПОРТОВ -----------------------------------
const int chipSelect = 9;          // Порт, на котором CD - карта
#define PIN_BUTTON_SET  2           // указываем номер вывода arduino, к которому подключена кнопка SET
#define PIN_BUTTON_UP   3           // указываем номер вывода arduino, к которому подключена кнопка UP
#define PIN_BUTTON_DOWN 4           // указываем номер вывода arduino, к которому подключена кнопка DOWN
//-------------------------------- ОБЪЯВЛЕНИЕ ПОРТОВ -----------------------------------

//-------------------------------- НАСТРОЙКА МОДУЛЕЙ -----------------------------------
MicroDS3231 rtc;                         // объявляем  переменную для работы с RTC модулем,  указывая название модуля
//iarduino_RTC rtc1(RTC_DS3231);
LiquidCrystal_I2C lcd(0x27, 16, 2);      // put your pin numbers here
GyverMAX6675_SPI<10> sens;               // программный SPI

#define MAX6675_SPI_SPEED 300000         // для увеличения качества связи по длинным проводам
//-------------------------------- НАСТРОЙКА МОДУЛЕЙ -----------------------------------

//-------------------------------- ОБЪЯВЛЕНИЕ ГЛОБАЛЬНЫХ ПЕРЕМЕННЫХ -----------------------------------
unsigned long t;      
unsigned long t1;                   //Создание глобальной переменной для времени
String fileName = "";
File myFile;
//-------------------------------- ОБЪЯВЛЕНИЕ ГЛОБАЛЬНЫХ ПЕРЕМЕННЫХ -----------------------------------


void setup() {
  Serial.begin(9600);                    // Открываем порт для монитора порта
  lcd.init();  
  //lcd.backlight();                         // Инициализация дисплея
  
  if (sens.readTemp()) {                                     // Читаем температуру
    Serial.println("Successful initizlizing sensor");        // Успешно - выводим лог
    lcd.print("Init successful");                            // Успешно - выводим на дисплей
  } 
  else {
    Serial.println("Error initizlizing sensor");             // ошибка чтения или подключения - выводим лог
    lcd.print("Init failed ");                               // Ошибка - выводим на дисплей
  }
  delay(500);                                                // Ждем 1,5 секунды 
  lcd.clear();                                               // Очищаем дисплей

  Serial.print("Initializing SD card...");

  if (!SD.begin(chipSelect)) {                            // Проверка на наличие СD карты
    Serial.println("Card failed, or not present");        // Вывод в монитор, что карта отсутствует или не читается
    lcd.print("Card failed");
    while(!SD.begin(chipSelect));                         // Ничего не делаем, пока карта не начнет читатся
  }
  Serial.println("card initialized.");                    // Вывод в монитор, что карта инициализировалась
  delay(1500);                                            // Ждем 1,5 секунды
  lcd.clear();                                            // Очищаем дисплей

  fileName = "logger.txt";

  // open a new file and immediately close it:
  Serial.println("Creating file");
  myFile = SD.open(fileName, FILE_WRITE);
  myFile.close();
  Serial.println(fileName);
}

void loop() {
  myFile = SD.open(fileName, FILE_WRITE);
  if (myFile) {                            // Если файл открылся
        myFile.println("The SD-CARD was pulled in");
  }
  myFile.close();
  while(SD.begin(chipSelect)) {                
      lcd.setCursor(0, 0);                     // устанавливаем курсор в позицию: столбец 0, строка 0
      lcd.print(rtc.getTimeString());          // выводим время
      lcd.setCursor(0, 1);                     // устанавливаем курсор в позицию: столбец 0, строка 1
      lcd.print(sens.getTemp());               // выводим температуру
      lcd.setCursor(5, 1);                     // устанавливаем курсор в позицию: столбец 5, строка 1
      lcd.print("*C");                         // выводим значок градуса
      Serial.println(rtc.getTimeString() + "          Temp: " + sens.getTemp() + " ºC"); // Отслеживание параметров в мониторе порта
  
      myFile = SD.open(fileName, FILE_WRITE);  // Открываем файл
      if (myFile) {                            // Если файл открылся
        myFile.print(rtc.getDateString() + "; " + rtc.getTimeString() + "; Temp " + sens.getTemp());      // выводим время
        myFile.println(" ");      // выводим время
        myFile.close();           // Закрываем файл
      } 
      else {
        Serial.print(fileName + ".txt     ");            // Иначе выводим в порт отладки ошибку
        Serial.println("Couldn't open example file");
      }
      delay(1000);                     // Каждую секунду
  }
  Serial.println("Card failed, or not present");        // Вывод в монитор, что карта отсутствует или не читается
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Card failed");
  while(!SD.begin(chipSelect));
  lcd.clear();


  
}
