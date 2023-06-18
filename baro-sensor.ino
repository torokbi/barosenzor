#include <Wire.h>
#include <Adafruit_BMP085.h>

Adafruit_BMP085 bmp;

void setup() {
  Serial.begin(9600); // inicializáljuk a soros portot
  if (!bmp.begin()) {
    Serial.println("A BMP180 szenzor nem található!");
    while (1);
  }
}

void loop() {
  float pressure = bmp.readPressure() / 100.0F; // olvassuk a légnyomást és számoljuk át hPa-ba
  float temperature = bmp.readTemperature(); // hőmérséklet kiolvasása
  temperature = (temperature - 32.0) * 5.0 / 9.0;  // hőmérséklet átszámítása celziusba

  String str = "POV,P,";
  Serial.print("$POV,P,");
  
  //Nyomásadat elküldése
  if (pressure > 0){
    Serial.print("+");
    str = str + "+";
    Serial.print(pressure);
    str = str + String(pressure)+"";
  }
  else {
    Serial.print(pressure);
    str = str + String(pressure);
  }

  //hőmérséklet elküldése
  Serial.print(",T,");
  str = str + ",T,";

  if (bmp.readTemperature() > 0){
    Serial.print("+");
    str = str + "+";
    Serial.print(bmp.readTemperature());
    str = str + String(bmp.readTemperature())+"";
  }
  else{
    Serial.print(bmp.readTemperature());
    str = str + String(bmp.readTemperature())+"";
  }

  //záró rész elküldése
  Serial.print("*");
  Serial.println(updateChecksum(str));
  //Serial.println(str);
  delay(200);
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
