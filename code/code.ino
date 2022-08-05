#include <SoftwareSerial.h>
#include <NewPing.h>
#include <HX711_ADC.h>
#include <EEPROM.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <DFPlayer_Mini_Mp3.h>

LiquidCrystal_I2C lcd(0x27,16,2); 
SoftwareSerial mySerial(10, 11); 


#define button 2
#define TRIGGER_PIN  6  // Arduino pin tied to trigger pin on the ultrasonic sensor.
#define ECHO_PIN     7  // Arduino pin tied to echo pin on the ultrasonic sensor.
#define MAX_DISTANCE 200 // Maximum distance we want to ping for (in centimeters). Maximum sensor distance is rated at 400-500cm.

const int HX711_dout = 9; //mcu > HX711 dout pin
const int HX711_sck = 8; //mcu > HX711 sck pin


NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE); // NewPing setup of pins and maximum distance.
HX711_ADC LoadCell(HX711_dout, HX711_sck);

long duration;
int tinggi;
int berat;
String result;
int T;
int B;
int bmi;
int x;
int p;
float i;

const int calVal_eepromAdress = 0;
unsigned long t = 0;

void setup()
{
  Serial.begin(9600);
  LoadCell.begin();
  boolean _tare = true;
  unsigned long stabilizingtime = 2000; 
  LoadCell.start(stabilizingtime, _tare);
  
  if (LoadCell.getTareTimeoutFlag() || LoadCell.getSignalTimeoutFlag()) {
    Serial.println("Timeout, check MCU>HX711 wiring and pin designations");
    while (1);
  }
  else {
    LoadCell.setCalFactor(1.0); 
    Serial.println("Startup is complete");
  }
  while (!LoadCell.update());
  calibrate(); 
  
  lcd.init();                  
  lcd.backlight();
  mySerial.begin (9600);
  mp3_set_serial (mySerial);
  mp3_set_volume (50);
  pinMode(button,INPUT_PULLUP);
}


void loop()
{
  static boolean newDataReady = 0;
  const int serialPrintInterval = 0; 

  if (LoadCell.update()) newDataReady = true;

  if (newDataReady) {
    if (millis() > t + serialPrintInterval) {
      float i = LoadCell.getData();
      Serial.print("Load_cell output val: ");
      Serial.println(i);
      newDataReady = 0;
      t = millis();
    }
  }

  p=sonar.ping_cm();
  T=p-200;
  B=i/1000;
  x=T*0.01;
  bmi=B/(x*x);
  
  Serial.print("Tinggi: ");
  Serial.println(T);
  Serial.print("Berat : ");
  Serial.println(B);
  Serial.print("BMI : ");
  Serial.println(bmi);

  lcd.setCursor(2,0);
  lcd.print("Tekan Tombol");
  lcd.setCursor(5,1);
  lcd.print("<<=>>");

  if (digitalRead(button) == 0 )
  {
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("T:");
    lcd.print(T);
    lcd.print("cm  ");
    lcd.print("B:");
    lcd.print(B);
    lcd.print("Kg");


    mp3_play(201);
    delay(2000);
    mp3_play(T);
    delay(3000);
    mp3_play(202);
    delay(2000);
    mp3_play(B);
    delay(3000);
    
    if (bmi < 18)
        {
          result = "BMI: Underweight";       
          lcd.setCursor(0,1);
          lcd.print(result); 
          mp3_play(207);
          delay(3500);
          mp3_play(203);
          delay(7000);
        }
     else if (bmi >=18 && bmi <=23) 
        {
          result = "BMI: Normal";
          lcd.setCursor(0,1);
          lcd.print(result);
          mp3_play(207);
          delay(3500);
          mp3_play(204);
          delay(7000);
        }
     else if (bmi >23 && bmi <=28) 
        {
         result = "BMI: Overweight";  
         lcd.setCursor(0,1);
         lcd.print(result);
         mp3_play(207);
         delay(3500);
         mp3_play(205);
         delay(7000);
        }
     else if(bmi >28) 
        {
         result = "BMI: Obesitas";
         lcd.setCursor(0,1);
         lcd.print(result);
         mp3_play(207);
         delay(3500);
         mp3_play(206);
         delay(7000);
        }
  
    lcd.clear();
  }
}
void calibrate() {
  Serial.println("***");
  Serial.println("Start calibration:");
  Serial.println("Place the load cell an a level stable surface.");
  Serial.println("Remove any load applied to the load cell.");
  Serial.println("Send 't' from serial monitor to set the tare offset.");

  boolean _resume = false;
  while (_resume == false) {
    LoadCell.update();
    if (Serial.available() > 0) {
      if (Serial.available() > 0) {
        char inByte = Serial.read();
        if (inByte == 't') LoadCell.tareNoDelay();
      }
    }
    if (LoadCell.getTareStatus() == true) {
      Serial.println("Tare complete");
      _resume = true;
    }
  }

  Serial.println("Now, place your known mass on the loadcell.");
  Serial.println("Then send the weight of this mass (i.e. 100.0) from serial monitor.");

  float known_mass = 0;
  _resume = false;
  while (_resume == false) {
    LoadCell.update();
    if (Serial.available() > 0) {
      known_mass = Serial.parseFloat();
      if (known_mass != 0) {
        Serial.print("Known mass is: ");
        Serial.println(known_mass);
        _resume = true;
      }
    }
  }

  LoadCell.refreshDataSet(); //refresh the dataset to be sure that the known mass is measured correct
  float newCalibrationValue = LoadCell.getNewCalibration(known_mass); //get the new calibration value

  Serial.print("New calibration value has been set to: ");
  Serial.print(newCalibrationValue);
  Serial.println(", use this as calibration value (calFactor) in your project sketch.");
  Serial.print("Save this value to EEPROM adress ");
  Serial.print(calVal_eepromAdress);
  Serial.println("? y/n");

  _resume = false;
  while (_resume == false) {
    if (Serial.available() > 0) {
      char inByte = Serial.read();
      if (inByte == 'y') {
        EEPROM.get(calVal_eepromAdress, newCalibrationValue);
        Serial.print("Value ");
        Serial.print(newCalibrationValue);
        Serial.print(" saved to EEPROM address: ");
        Serial.println(calVal_eepromAdress);
        _resume = true;

      }
      else if (inByte == 'n') {
        Serial.println("Value not saved to EEPROM");
        _resume = true;
      }
    }
  }

  Serial.println("End calibration");
  Serial.println("***");
}
