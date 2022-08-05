#include <HX711_ADC.h>
#include <EEPROM.h>

//pins:
const int HX711_dout = 4; //mcu > HX711 dout pin
const int HX711_sck = 5; //mcu > HX711 sck pin

//HX711 constructor:
HX711_ADC LoadCell(HX711_dout, HX711_sck);

const int calVal_eepromAdress = 0;
float newCalibrationValue= 79;


void setup()
{
      LoadCell.begin();
}

void loop() 
{
      float i = LoadCell.getData();
}
