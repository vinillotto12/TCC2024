


void openFS(void);
void verificaNTP();
void scan_dht22();

unsigned long int t_loop = millis();
bool gsm_habilitado = false;


//DHT22
unsigned long int t_dht22 = millis();
#define DHTPIN 4
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

float temperatura = 0;
float temp_min = 10;
float tem_max = 60;
bool alarme_temperatura = false;
bool temp_flag = false;
bool envia_alarme_temp = false;

float umidade = 0;
float umidade_min = 20;
float umidade_max = 99;
bool alarme_umidade = false;
bool envia_alarme_umid = false;


//NTP

//ACELEROMETRO
Adafruit_MPU6050 mpu;
float accelerometer_x;
float accelerometer_y;
float accelerometer_z;
float gyro_x;
float gyro_y;
float gyro_z;
int escala_accelerometer[3];
unsigned long int t_mpu = millis();
void setupMPU();
void scan_mpu();


//GPS
boolean carro_parado = false;
boolean gps_run = false;

void setupMPU() {
  //GYRO
  Serial.println("INICIANDO MPU");

  if (!mpu.begin()) {
    Serial.println(" - FALHA, MPU NAO ENCONTRADO");
  } else {
    Serial.println("MPU6050 OK");

    mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
    Serial.print("Accelerometer range set to: ");
    switch (mpu.getAccelerometerRange()) {
      case MPU6050_RANGE_2_G:
        Serial.println("+-2G");
        break;
      case MPU6050_RANGE_4_G:
        Serial.println("+-4G");
        break;
      case MPU6050_RANGE_8_G:
        Serial.println("+-8G");
        break;
      case MPU6050_RANGE_16_G:
        Serial.println("+-16G");
        break;
    }
    mpu.setGyroRange(MPU6050_RANGE_500_DEG);
    Serial.print("Gyro range set to: ");
    switch (mpu.getGyroRange()) {
      case MPU6050_RANGE_250_DEG:
        Serial.println("+- 250 deg/s");
        break;
      case MPU6050_RANGE_500_DEG:
        Serial.println("+- 500 deg/s");
        break;
      case MPU6050_RANGE_1000_DEG:
        Serial.println("+- 1000 deg/s");
        break;
      case MPU6050_RANGE_2000_DEG:
        Serial.println("+- 2000 deg/s");
        break;
    }

    mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
    Serial.print("Filter bandwidth set to: ");
    switch (mpu.getFilterBandwidth()) {
      case MPU6050_BAND_260_HZ:
        Serial.println("260 Hz");
        break;
      case MPU6050_BAND_184_HZ:
        Serial.println("184 Hz");
        break;
      case MPU6050_BAND_94_HZ:
        Serial.println("94 Hz");
        break;
      case MPU6050_BAND_44_HZ:
        Serial.println("44 Hz");
        break;
      case MPU6050_BAND_21_HZ:
        Serial.println("21 Hz");
        break;
      case MPU6050_BAND_10_HZ:
        Serial.println("10 Hz");
        break;
      case MPU6050_BAND_5_HZ:
        Serial.println("5 Hz");
        break;
    }
    Serial.println("");
  }
}


void scan_mpu() {
  if (millis() - t_mpu > 1000) {
    t_mpu = millis();

    sensors_event_t a, g, temp;
    mpu.getEvent(&a, &g, &temp);

    accelerometer_x = a.acceleration.x;
    accelerometer_y = a.acceleration.y;
    accelerometer_z = a.acceleration.z;
    Serial.println(" REAL ");
    Serial.print(" X -> "); Serial.println(accelerometer_x);
    Serial.print(" Y -> "); Serial.println(accelerometer_y);
    Serial.print(" Z -> "); Serial.println(accelerometer_z);
    escala_accelerometer[0] = map(accelerometer_x, 0, 50, 0, 100);//AJUSTE DE ESCALA
    escala_accelerometer[1] = map(accelerometer_y, 0, 50, 0, 100);//
    escala_accelerometer[2] = map(accelerometer_z, 0, 50, 0, 100);//
    Serial.println(" LINER ");
    Serial.print(" X -> "); Serial.println(escala_accelerometer[0]);
    Serial.print(" Y -> "); Serial.println(escala_accelerometer[1]);
    Serial.print(" Z -> "); Serial.println(escala_accelerometer[2]);
  }
}



//REALIZA A LEITURA A CADA SEGUNDOS
void scan_dht22() {
  if (millis() - t_dht22 > 5000) {
    t_dht22 = millis();

    temperatura = dht.readTemperature();
    if ((temperatura < temp_min || temperatura > tem_max)) {
      if (!alarme_temperatura) {
        alarme_temperatura = true;
        envia_alarme_temp = true;
        gsm_habilitado = true;
        Serial.print("Temperatura alarme \n");
      }
    } else {
      alarme_temperatura = false;
    }

    umidade = dht.readHumidity();
    if (umidade < umidade_min || umidade > umidade_max) {
      if (!alarme_umidade) {
        alarme_umidade = true;
        envia_alarme_umid = true;
        gsm_habilitado = true;
        Serial.print("Umidade alarme \n");
      }
    } else {
      alarme_umidade = false;
    }
    // Serial.print("Temperatura ");
    // Serial.println(temperatura);


    // Serial.print("Humidade ");
    // Serial.println(umidade);
  }
}

void openFS(void) {

  String st_log = "";

  if (false) {
    st_log = " - FORMATANDO FLASH.";
    // SPIFFS.format();
  }

  if (!SPIFFS.begin(true)) {
    st_log = "- Erro ao abrir FLASH.";
    ESP.restart();
  } else {
    st_log = "- FLASH aberto com sucesso.";
  }


  Serial.println(st_log);
}
