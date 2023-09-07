#include <Wire.h>
#include <Adafruit_BMP085.h>
#include "BluetoothSerial.h"

Adafruit_BMP085 bmp;
BluetoothSerial SerialBT;
const char *pin = "1234";
float datas[10]={};

void setup() {

  // setup serialmonitor and BT
  Serial.begin(115200);
  SerialBT.begin("Baro");
  //SerialBT.setPin(pin);

  // setup BMP180
  if (!bmp.begin()) {
    while (1){
      SerialBT.println("A BMP180 szenzor nem található!");
    };
  }

  // make the avg list
  int index = 0;
  float sum = 0, avgsetup = 0;
  do{
    float pressure = bmp.readPressure() / 100.0F;
    
    if (index == 0){
      datas[index] = pressure;
      index++;
      continue;
    }
    for (int i=0; i < index; i++){
      sum = sum + datas[i];
    }
    if (index != 1){
      avgsetup = sum / (index-1);
    }
    else{
      avgsetup = sum;
    }
    if (pressure < avgsetup*1,5){
      datas[index] = pressure;
      index++;
    }
    delay(200);
  } while(index < 10);

  SerialBT.print("Kezdeti nyomásátlag: ");
  SerialBT.println(String(avg(datas))+"HPa");
}

void loop() {
  int lastindex = 0;

  for (int i = lastindex; i < 10; i++){
    float sum = 0;
    
    // messure
    float pressure = bmp.readPressure() / 100.0F;
    float temperature = bmp.readTemperature();
    

    // calculate the avg of datas
    for (int a=0; a < 10; a++){
      sum = sum + datas[i];
    }
  

    // chek the pressure is correct
    if (pressure > avg(datas)*1.4){
      lastindex = i;
      break;
    }
    // send the beginning
    String str = "POV,P,";
    SerialBT.print("$POV,P,");
    
    // send pressure
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

    // send temperature
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

    // send checksum
    SerialBT.print("*");
    SerialBT.println(updateChecksum(str));

    // refresh the datas list and reset lastindex
    datas[i] = pressure;
    if (i == 9){
      lastindex = 0;
    }

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

float avg(float datalist[10]){
  float summa = 0;
  for (int index = 0; index < 10; index ++){
    summa = summa + datalist[index];
  }
  return summa/10;
}
