//-------------------------------- Libraries -----------------------------------
#include <SD.h>
#include <SPI.h>
#include <LiquidCrystal_I2C.h>      // https://www.arduino.cc/reference/en/libraries/liquidcrystal-i2c/ 1.1.2
#include <Wire.h>                   // подключаем библиотеку "Wire" - Стандартная библиотека
//#include <GyverMAX6675.h>
#include <GyverMAX6675_SPI.h>       // https://github.com/GyverLibs/GyverMAX6675
#include <charMap.h>                // https://github.com/GyverLibs/GyverOLED
#include <GyverGFX.h>               // https://github.com/GyverLibs/GyverGFX
#include <iarduino_RTC.h>           // подключаем библиотеку для работы с RTC модулем https://github.com/tremaru/iarduino_RTC/archive/2.0.0.zip
//-------------------------------- Libraries -----------------------------------

//-------------------------------- ОБЪЯВЛЕНИЕ ПОРТОВ -----------------------------------
#define PIN_CHIP_SELECT 7           // Порт, на котором CD - карта
#define PIN_BUTTON_SET  2           // указываем номер вывода arduino, к которому подключена кнопка SET
#define PIN_BUTTON_UP   3           // указываем номер вывода arduino, к которому подключена кнопка UP
#define PIN_BUTTON_DOWN 4           // указываем номер вывода arduino, к которому подключена кнопка DOWN
//-------------------------------- ОБЪЯВЛЕНИЕ ПОРТОВ -----------------------------------

//-------------------------------- НАСТРОЙКА МОДУЛЕЙ -----------------------------------
iarduino_RTC rtc(RTC_DS3231);            // объявляем  переменную для работы с RTC модулем,  указывая название модуля
LiquidCrystal_I2C lcd(0x27, 16, 2);      // put your pin numbers here
GyverMAX6675_SPI<10> sens;   // программный SPI

uint8_t temp_cel[8] =
{
B00111,
B00101,
B00111,
B00000,
B00000,
B00000,
B00000
}; //закодировано в двоичной системе значек градуса
//-------------------------------- НАСТРОЙКА МОДУЛЕЙ -----------------------------------

//-------------------------------- ОБЪЯВЛЕНИЕ ГЛОБАЛЬНЫХ ПЕРЕМЕННЫХ -----------------------------------
uint8_t VAR_mode_SHOW   = 1;           // режим вывода: 1-время 2-дата
uint8_t VAR_mode_SET    = 0;           // режим установки времени: 0-нет 1-сек 2-мин 3-час 4-день 5-мес 6-год 7-день_недели
unsigned long t;
unsigned long t1;
//-------------------------------- ОБЪЯВЛЕНИЕ ГЛОБАЛЬНЫХ ПЕРЕМЕННЫХ -----------------------------------

void setup(){
  pinMode(PIN_BUTTON_SET,  INPUT_PULLUP);           // устанавливаем режим работы вывода кнопки, как "вход"
  pinMode(PIN_BUTTON_UP,   INPUT_PULLUP);           // устанавливаем режим работы вывода кнопки, как "вход"
  pinMode(PIN_BUTTON_DOWN, INPUT_PULLUP);           // устанавливаем режим работы вывода кнопки, как "вход"
  lcd.begin(8, 2);                           // put your LCD parameters here;
  Wire.begin();                              // подключаемся к шине I2C как мастер
  pinMode(PIN_CHIP_SELECT, OUTPUT);          // Устанавливаем CD - карту, как "вывод"
  Serial.begin(9600);                        // Пробрасываем порт для отладки
  lcd.createChar(1, temp_cel);               // Создаем новый символ - градусы

  if(SD.exists("LOG.txt")) {
    SD.remove("LOG.txt");
  }
  
  if(sens.readTemp()) Serial.println("Sensor available");       // Проверка доступности датчика температуры
  else Serial.println("Sensor is not available");  
  
  
  if (!SD.begin(PIN_CHIP_SELECT)) {         // Проверка доступности CD - карты
    Serial.println("Card failed, or not present");
    return;
  }
  Serial.println("card initialized.");
}

void loop() {

  if(millis()%1000==0){                      // если прошла 1 секунда
      lcd.setCursor(0, 0);                   // устанавливаем курсор в позицию: столбец 0, строка 0
    if(VAR_mode_SHOW==1){                    // если установлен режим вывода времени
      lcd.print(rtc.gettime("H:i:s"));      // выводим время
    }
    if(VAR_mode_SHOW==2){                    // если установлен режим вывода даты
      lcd.print(rtc.gettime("d-m-Y   D"));  // выводим дату
    }
        if(sens.readTemp()) {
      int temp = sens.getTempInt();
      Serial.println(temp);
      lcd.setCursor(2, 1);
      lcd.print(temp);
      lcd.setCursor(5, 1);
      char cel[]={'\1', 67, '\0'}; //Отрисовка градусов Цельсия
      lcd.print(cel);
      Serial.println(temp);

      
  if(millis() - t1 >= 1000) {
    t1 = millis();
    File logFile = SD.open("LOG.txt", FILE_WRITE);
    if (logFile) {
      logFile.print(rtc.gettime("H:i:s "));      // выводим время
      logFile.print(rtc.gettime("d-m-Y"));  // выводим дату
      /*logFile.print(rtc.hours());
      logFile.print(":");
      logFile.print(rtc.minutes());
      logFile.print(":");
      logFile.print(rtc.seconds());
      logFile.print(" ");
      logFile.print(rtc.date());
      logFile.print("/");
      logFile.print(rtc.month());
      logFile.print("/");
      logFile.print(rtc.year());
      logFile.print(" ");
      */

      logFile.println(sens.getTempInt());  
      logFile.close();
      Serial.println("good");
     } 
     else {
      Serial.println("LOG.txt");
      Serial.println("Couldn't open log file");
     }
    }   
    delay(1);                                // приостанавливаем на 1 мс, чтоб не выводить время несколько раз за 1мс
  }
  Func_buttons_control();                    // передаём управление кнопкам
}
}

void Func_buttons_control(){
  uint8_t i=0;
  rtc.blinktime(VAR_mode_SET);                                  // мигаем устанавливаемым параметром (если VAR_mode_SET больше 0)
//Если часы находятся в режиме установки даты/времени
  if(VAR_mode_SET){
//  Если нажата кнопка UP
    if(digitalRead(PIN_BUTTON_UP)){
      while(digitalRead(PIN_BUTTON_UP)){delay(50);}              // ждём пока мы не отпустим кнопку UP
      switch (VAR_mode_SET){                                     // инкремент (увеличение) устанавливаемого значения
      /* сек */ case 1: rtc.settime(0,                                   -1, -1, -1, -1, -1, -1); break;
      /* мин */ case 2: rtc.settime(-1, (rtc.minutes==59?0:rtc.minutes+1), -1, -1, -1, -1, -1); break;
      /* час */ case 3: rtc.settime(-1, -1, (rtc.Hours==23?0:rtc.Hours+1),     -1, -1, -1, -1); break;
      /* дни */ case 4: rtc.settime(-1, -1, -1, (rtc.day==31?1:rtc.day+1),         -1, -1, -1); break;
      /* мес */ case 5: rtc.settime(-1, -1, -1, -1, (rtc.month==12?1:rtc.month+1),     -1, -1); break;
      /* год */ case 6: rtc.settime(-1, -1, -1, -1, -1, (rtc.year==99?0:rtc.year+1),       -1); break;
      /* д.н.*/ case 7: rtc.settime(-1, -1, -1, -1, -1, -1, (rtc.weekday==6?0:rtc.weekday+1) ); break;
      }
    }
//  Если нажата кнопка DOWN
    if(digitalRead(PIN_BUTTON_DOWN)){
      while(digitalRead(PIN_BUTTON_DOWN)){delay(50);}            // ждём пока мы её не отпустим
      switch (VAR_mode_SET){                                     // декремент (уменьшение) устанавливаемого значения
      /* сек */ case 1: rtc.settime(0,                                   -1, -1, -1, -1, -1, -1); break;
      /* мин */ case 2: rtc.settime(-1, (rtc.minutes==0?59:rtc.minutes-1), -1, -1, -1, -1, -1); break;
      /* час */ case 3: rtc.settime(-1, -1, (rtc.Hours==0?23:rtc.Hours-1),     -1, -1, -1, -1); break;
      /* дни */ case 4: rtc.settime(-1, -1, -1, (rtc.day==1?31:rtc.day-1),         -1, -1, -1); break;
      /* мес */ case 5: rtc.settime(-1, -1, -1, -1, (rtc.month==1?12:rtc.month-1),     -1, -1); break;
      /* год */ case 6: rtc.settime(-1, -1, -1, -1, -1, (rtc.year==0?99:rtc.year-1),       -1); break;
      /* д.н.*/ case 7: rtc.settime(-1, -1, -1, -1, -1, -1, (rtc.weekday==0?6:rtc.weekday-1) ); break;
      }
    }
//  Если нажата кнопка SET
    if(digitalRead(PIN_BUTTON_SET)){
      while(digitalRead(PIN_BUTTON_SET)){                        // ждём пока мы её не отпустим
        delay(10);
        if(i<200){i++;}else{lcd.clear();}                        // фиксируем, как долго удерживается кнопка SET, если дольше 2 секунд, то стираем экран
      }
      if(i<200){                                                 // если кнопка SET удерживалась меньше 2 секунд
        VAR_mode_SET++;                                          // переходим к следующему устанавливаемому параметру
        if(VAR_mode_SHOW==1 && VAR_mode_SET>3){VAR_mode_SET=1;}  // возвращаемся к первому устанавливаемому параметру
        if(VAR_mode_SHOW==2 && VAR_mode_SET>7){VAR_mode_SET=4;}  // возвращаемся к первому устанавливаемому параметру
      }else{                                                     // если кнопка SET удерживалась дольше 2 секунд, то требуется выйти из режима установки даты/времени
        VAR_mode_SET=0;                                          // выходим из режима установки даты/времени
      }
    }
//Если часы находятся в режиме вывода даты/времени
  }else{
//  Если нажата кнопка SET
    if(digitalRead(PIN_BUTTON_SET)){
      while(digitalRead(PIN_BUTTON_SET)){delay(10);              // ждём пока мы её не отпустим
        if(i<200){i++;}else{lcd.clear();}                        // фиксируем, как долго удерживается кнопка SET, если дольше 2 секунд, то стираем экран
      }
      if(i<200){                                                 // если кнопка SET удерживалась меньше 2 секунд
        lcd.clear();                                             // стираем экран
        VAR_mode_SHOW=VAR_mode_SHOW==1?2:1;                      // меняем режим вывода: дата/время
      }else{                                                     // если 
        if(VAR_mode_SHOW==1){VAR_mode_SET=1;}                    // если установлен режим вывода времени, то устанавливаемый параметр - секунды
        if(VAR_mode_SHOW==2){VAR_mode_SET=4;}                    // если установлен режим вывода даты,    то устанавливаемый параметр - день
      }
    }
  }
}
