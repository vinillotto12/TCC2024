#define RXD1 33
#define TXD1 32

static const uint32_t GPSBaud = 9600;

TinyGPSPlus gps;

void setupGPS();
void displayInfo();


float latitude = 0;
float longitude = 0 ;
unsigned long int t_gps = millis();


void setupGPS() {
  Serial1.begin(GPSBaud , SERIAL_8N1, RXD1, TXD1);
}


//TASK
void Task_GPS( void *pvParameters )
{
  (void) pvParameters;
  setupGPS();
  //UBaseType_t uxHighWaterMark;
  while (1)
  {


    while (Serial1.available() > 0)
      if (gps.encode(Serial1.read()))
        displayInfo();

    if (millis() > 5000 && gps.charsProcessed() < 10)
    {
      Serial.println(F("No GPS detected: check wiring."));
      gps_run = false;
      while (true);
    }

    vTaskDelay(500 / portTICK_PERIOD_MS);
  }
}


void displayInfo()
{
  Serial.print(F("Location: "));
  if (gps.location.isValid())
  {
    gps_run = true;
    float lati = gps.location.lat();
    float lon = gps.location.lng();
    float ele = 0.0;
    Serial.print(lati);
    Serial.print(F(","));
    Serial.print(lon);

    if (lati != latitude) {
      latitude = 0;
      longitude = 0;
      carro_parado = false;
      t_gps = millis();
    } else {
      if (millis() - t_gps > 5000) {
        carro_parado = true;
      }
    }




  }
  else
  {
    Serial.print(F("INVALID"));
  }
  Serial.println();
}
