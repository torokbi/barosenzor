#include <Wire.h>
#include <Adafruit_BMP085.h>
#include "BluetoothSerial.h"

Adafruit_BMP085 bmp;
BluetoothSerial SerialBT;
const char *pin = "1234";
float datas[10]={};

void setup() {
  Serial.begin(115200);
  SerialBT.begin("Baro");
  //SerialBT.setPin(pin);

  if (!bmp.begin()) {
    while (1){
      SerialBT.println("A BMP180 szenzor nem található!");
    };
  }

  for (int i=0; i < 10; i++) {
    float pressure = bmp.readPressure() / 100.0F;
    datas[i] = pressure;
    
    delay(200);
  }
}

void loop() {
  for (int i=0; i < 10; i++){
    float pressure = bmp.readPressure() / 100.0F; // olvassuk a légnyomást és számoljuk át hPa-ba
    float temperature = bmp.readTemperature(); // hőmérséklet kiolvasása
    //temperature = (temperature - 32.0) * 5.0 / 9.0;  // hőmérséklet átszámítása celziusba
    
    float sum = 0;
    float avg = 0;

    for (int a=0; a < 10; a++){
      sum = sum + datas[i];
    }
    avg = sum / 10;
  
  if (pressure > avg*1.4){
    break;
  }
    String str = "POV,P,";
    SerialBT.print("$POV,P,");
    
    //Nyomásadat elküldése
    if (pressure > 0){
      SerialBT.print("+");
      str = str + "+";
      SerialBT.print(pressure);
      str = str + String(pressure)+"";
    }
    else {
      SerialBT.print(pressure);
      str = str + String(pressure);
    }

    //hőmérséklet elküldése
    SerialBT.print(",T,");
    str = str + ",T,";

    if (temperature > 0){
      SerialBT.print("+");
      str = str + "+";
      SerialBT.print(temperature);
      str = str + String(temperature)+"";
    }
    else{
      SerialBT.print(temperature);
      str = str + String(temperature)+"";
    }

    //záró rész elküldése
    SerialBT.print("*");
    SerialBT.println(updateChecksum(str));

    datas[i] = pressure;

    delay(200);
  }
}

String updateChecksum(String cmd)
{
  // Compute the checksum by XORing all the character values in the string.
  byte checksum = 0;
  for (int i = 0; i < cmd.length(); i++) {
    checksum = checksum ^ cmd.charAt(i);
  }

  // Convert it to hexadecimal (base-16, upper case, most significant nybble first).
  String hexsum = String(checksum, HEX);
  if (hexsum.length() < 2) {
    hexsum = "0" + hexsum;
  }
  
  return hexsum;
}
