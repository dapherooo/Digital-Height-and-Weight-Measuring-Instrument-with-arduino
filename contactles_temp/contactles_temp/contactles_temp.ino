#include <Wire.h>
#include <SoftwareSerial.h>
#include <LiquidCrystal_I2C.h> //include melalaui file
#include <DFPlayer_Mini_Mp3.h> //include melalui file
#include <Adafruit_MLX90614.h> //instal di library manager
LiquidCrystal_I2C lcd (0x27, 16, 2);
SoftwareSerial mySerial(11, 10); //RX, TX
Adafruit_MLX90614 mlx = Adafruit_MLX90614();
int temp;
#define button 2
String result;

void setup() {
  Serial.begin(9600);
  Serial.println("Adafruit MLX90614 test");  
  mlx.begin();  
  lcd.begin();
  mySerial.begin (9600);
  mp3_set_serial (mySerial);
  mp3_set_volume (50);
  pinMode(button,INPUT_PULLUP);
  
}

void loop() {
//  lcd.setCursor (0,0);
//  lcd.print("Cek Suhu Tubuh: ");
  
  temp = mlx.readObjectTempC();
  Serial.print("tObject = "); Serial.print(temp); Serial.println("*C");
  Serial.println();

  if (temp >= 38){
    result = "Suhu Bahaya";
  }else {
    result = "Suhu Aman";
  }

    lcd.setCursor (0,0);
    lcd.print("Suhu : ");
    lcd.print(temp);
    lcd.print((char)223);
    lcd.print("C    ");
    lcd.setCursor (0,1);
    lcd.print(result);

  if (digitalRead(button) == 0 ){
    lcd.setCursor (0,0);
    lcd.print("Suhu : ");
    lcd.print(temp);
    lcd.print((char)223);
    lcd.print("C    ");
    lcd.setCursor (0,1);
    lcd.print(result);
    mp3_play (temp);
    delay(2000);
    mp3_play(111);
    delay(2000);
    lcd.clear();
  }
  
}
