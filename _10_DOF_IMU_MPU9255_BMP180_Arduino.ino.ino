#include <Wire.h>
#include <SFE_BMP180.h>
SFE_BMP180 pressure;
double baseline;
void setup()
{
    Serial.begin(115200);
    Wire.begin();  // I2C haberlesme baslatir

     if (pressure.begin())
    Serial.println("BMP180 init success");
  else
    {
    // Tahminen baglanti hatası yuzundendir
    Serial.println("BMP180 init fail (disconnected?)\n\n");
    while(1); // Sonsuz dongu
  }
   baseline = getPressure();
  
  Serial.print("baseline pressure: ");
  Serial.print(baseline);
  Serial.println(" mb");  
  delay(500);
  
    write(0x6B, 0); //Guc yonetimi registeri default:0
    write(0x6A, 0);  // I2C master kapali, acik olmasini istiyorsaniz 0x20 olmali
    write(0x37, 0x02); //Bypass modu acik
    writeMag(0x0A, 0x12); // surekli olcebilmek icin manyetik sensor registeri
}

void loop()
{
    double a,P;
  P = getPressure();
  a = pressure.altitude(P,baseline);
 Serial.print("relative altitude: ");
  if (a >= 0.0) Serial.print(" "); // pozitif sayilar icin bosluk olmali
  Serial.print(a,1);
  Serial.print(" meters, ");
  if (a >= 0.0) Serial.print(" "); // pozitif sayilar icin bosluk olmali
  Serial.print(a*3.28084,0);
  Serial.println(" feet");
    
    int xh = readMag(0x04); // x yonunu oku, high byte
    int xl = readMag(0x03); // x yonunu oku, low byte
    int yh = readMag(0x06);
    int yl = readMag(0x05);
    int zh = readMag(0x08);
    int zl = readMag(0x07);
              readMag(0x09); //Mag modul baska bir olcum yapmak icin 
    int x = (xh << 8) | (xl & 0xff);
    int y = (yh << 8) | (yl & 0xff);
    int z = (zh << 8) | (zl & 0xff);
    // Seri port cikislari
    Serial.print("X,Y,Z=");
    Serial.print(x);
    Serial.print(",");
    Serial.print(y);
    Serial.print(",");
    Serial.println(z);
    Serial.println("");
    Serial.println("");
   delay(1000); 
}
byte readMag(int reg)
{
    Wire.beginTransmission(0x0C);
    Wire.write(reg);
    Wire.endTransmission(false);
    Wire.requestFrom(0x0C, 1, false); // talep edilen data verisi
    byte val = Wire.read();
    Wire.endTransmission(true);
    return val;
}

void writeMag(int reg, int data)
{
    Wire.beginTransmission(0x0C);
    Wire.write(reg);
    Wire.write(data);
    Wire.endTransmission(true);
}
byte read(int reg)
{
    Wire.beginTransmission(0x68); // 0x68 sensor adresine veri transferi baslar
    Wire.write(reg);
    Wire.endTransmission(false);
    Wire.requestFrom(0x68, 1, false); // talep edilen data verisi
    byte val = Wire.read();
    Wire.endTransmission(true);
    return val;
}

void write(int reg, int data)
{
    Wire.beginTransmission(0x68); // 0x68 sensor adresine veri transferi baslar
    Wire.write(reg);
    Wire.write(data);
    Wire.endTransmission(true);
}
double getPressure()
{
  char status;
  double T,P,p0,a;

  // Sıcaklık olcumu baslar
  // eger istek basariliysa, olculen deger geri doner
  // eger istek basarisizsa, 0 doner.

  status = pressure.startTemperature();
  if (status != 0)
  {
    delay(status);

    // Retrieve the completed temperature measurement:
    // Note that the measurement is stored in the variable T.
    // Use '&T' to provide the address of T to the function.
    // Function returns 1 if successful, 0 if failure.

    status = pressure.getTemperature(T);
    if (status != 0)
    {
      // Start a pressure measurement:
      // The parameter is the oversampling setting, from 0 to 3 (highest res, longest wait).
      // If request is successful, the number of ms to wait is returned.
      // If request is unsuccessful, 0 is returned.

      status = pressure.startPressure(3);
      if (status != 0)
      {
        // Wait for the measurement to complete:
        delay(status);

        // Retrieve the completed pressure measurement:
        // Note that the measurement is stored in the variable P.
        // Use '&P' to provide the address of P.
        // Note also that the function requires the previous temperature measurement (T).
        // (If temperature is stable, you can do one temperature measurement for a number of pressure measurements.)
        // Function returns 1 if successful, 0 if failure.

        status = pressure.getPressure(P,T);
        if (status != 0)
        {
          return(P);
        }
        else Serial.println("error retrieving pressure measurement\n");
      }
      else Serial.println("error starting pressure measurement\n");
    }
    else Serial.println("error retrieving temperature measurement\n");
  }
  else Serial.println("error starting temperature measurement\n");
}



