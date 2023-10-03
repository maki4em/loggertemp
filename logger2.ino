//-------------------------------- Libraries -----------------------------------
#include <SD.h>                     // подключаем библиотеку для работы sd-card reader
#include <SPI.h>                    // подключаем библиотеку для работы шины SPI
#include <LiquidCrystal_I2C.h>      // подключаем библиотеку для работы с дисплеем https://www.arduino.cc/reference/en/libraries/liquidcrystal-i2c/ 
#include <Wire.h>                   // подключаем библиотеку "Wire" - Стандартная библиотека
#include <GyverMAX6675_SPI.h>       // подключаем библиотеку для работы c термопарой https://github.com/GyverLibs/GyverMAX6675
#include <GyverGFX.h>               // подключаем библиотеку для работы графики дисплея https://github.com/GyverLibs/GyverGFX
#include <microDS3231.h>            // подключаем библиотеку для работы с RTC модулем https://github.com/GyverLibs/microDS3231
//-------------------------------- Libraries -----------------------------------

//-------------------------------- ОБЪЯВЛЕНИЕ ПОРТОВ -----------------------------------
const int chipSelect = 7;          // Порт, на котором CD - карта
#define PIN_BUTTON_SET  2           // указываем номер вывода arduino, к которому подключена кнопка SET
#define PIN_BUTTON_UP   3           // указываем номер вывода arduino, к которому подключена кнопка UP
#define PIN_BUTTON_DOWN 4           // указываем номер вывода arduino, к которому подключена кнопка DOWN
//-------------------------------- ОБЪЯВЛЕНИЕ ПОРТОВ -----------------------------------

//-------------------------------- НАСТРОЙКА МОДУЛЕЙ -----------------------------------
MicroDS3231 rtc;;                        // объявляем  переменную для работы с RTC модулем,  указывая название модуля
LiquidCrystal_I2C lcd(0x27, 16, 2);      // put your pin numbers here
GyverMAX6675_SPI<10> sens;               // программный SPI

#define MAX6675_SPI_SPEED 300000         // для увеличения качества связи по длинным проводам

char cel[]={'\1', 67, '\0'};             // Создание символа Градусов Цельсия
//-------------------------------- НАСТРОЙКА МОДУЛЕЙ -----------------------------------

//-------------------------------- ОБЪЯВЛЕНИЕ ГЛОБАЛЬНЫХ ПЕРЕМЕННЫХ -----------------------------------
unsigned long t;      
unsigned long t1;                   //Создание глобальной переменной для времени
File myFile;
//-------------------------------- ОБЪЯВЛЕНИЕ ГЛОБАЛЬНЫХ ПЕРЕМЕННЫХ -----------------------------------


void setup() {
  Serial.begin(9600);                    // Открываем порт для монитора порта
  lcd.init();                            // Инициализация дисплея
  
  if (sens.readTemp()) {                 // Читаем температуру
    Serial.println("Successful");        // Успешно - выводим лог
    lcd.print("Init successful");        // Успешно - выводим на дисплей
  } 
  else {
    Serial.println("Error");             // ошибка чтения или подключения - выводим лог
    lcd.print("Init failed");            // Ошибка - выводим на дисплей
  }
  delay(1500);                           // Ждем 1,5 секунды
  lcd.clear();                           // Очищаем дисплей


  Serial.print("Initializing SD card...");

  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    while (1);
  }
  Serial.println("card initialized.");


    if (SD.exists("example.txt")) {
    Serial.println("example.txt exists.");
  } else {
    Serial.println("example.txt doesn't exist.");
  }

  // open a new file and immediately close it:
  Serial.println("Creating example.txt...");
  myFile = SD.open("example.txt", FILE_WRITE);
  myFile.close();

  // Check to see if the file exists:
  if (SD.exists("example.txt")) {
    Serial.println("example.txt exists.");
  } else {
    Serial.println("example.txt doesn't exist.");
  }
}

void loop() {
  if(millis()%1000==0){                      // Каждую секунду
      lcd.setCursor(0, 0);                   // устанавливаем курсор в позицию: столбец 0, строка 0
      lcd.print(rtc.getTimeString());        // выводим время
      lcd.setCursor(0, 1);                   // устанавливаем курсор в позицию: столбец 0, строка 1
      lcd.print(sens.getTemp());             // выводим температуру
      lcd.setCursor(5, 1);                   // устанавливаем курсор в позицию: столбец 5, строка 1
      lcd.print(cel);                        // выводим значок градуса
      Serial.println(rtc.getTimeString() + "          Temp: " + sens.getTemp() + " ºC"); // Отслеживание параметров в мониторе порта
  }

    if(millis() - t1 >= 1000) {
    t1 = millis();
    File myFile = SD.open("example.txt", FILE_WRITE);
    if (myFile) {
      myFile.print(rtc.getTimeString() + "          Temp: " + sens.getTemp() + " ºC");      // выводим время
      myFile.println(" ");      // выводим время
      myFile.close();
      Serial.println("good");
     } 
     else {
      Serial.println("example.txt");
      Serial.println("Couldn't open example file");
     }
    }   
    delay(1);                                // приостанавливаем на 1 мс, чтоб не выводить время несколько раз за 1мс
}
