#include <ArduinoJson.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
#include <TinyGPSPlus.h>
#include "FS.h"
#include "SPIFFS.h"
#include "DHT.h"
#include "SISTEMA.h"
#include "GSM.h"
#include "GPS.h"
#include "WIFI.h"
#include "MQTT.h"
#include "DATA.h"

// RTOS and Semaphore Definitions
SemaphoreHandle_t xFlash_semaphore = NULL;
SemaphoreHandle_t xConn_semaphore = NULL;

// Function Prototypes
void Task_Gerencia(void *pvParameters);
void Task_GPS(void *pvParameters);

void cria_diretorio(const String& path);
void listDir(const char* dirname, uint8_t levels);
void scan_envia_log();
void scan_novo_log();



void setup() {
    Serial.begin(115200);
    dht.begin();

    pinMode(MODEM_PWKEY, OUTPUT);
    pinMode(MODEM_RST, OUTPUT);
    pinMode(MODEM_POWER_ON, OUTPUT);
    gsm_setup();

    openFS();
    cria_diretorio("/enviar");
    listDir("/enviar", 1);

    setupMPU();

    // Semaphore Initialization
    xFlash_semaphore = xSemaphoreCreateMutex();
    xConn_semaphore = xSemaphoreCreateMutex();
    if (xFlash_semaphore == NULL) {
        Serial.println("- Falha ao inicializar semaforos|Filas. O programa nao pode prosseguir.");
        delay(2000);
        ESP.restart();
    } else {
        Serial.println("- Semaforos|Filas foram inicializadas com sucesso.");
    }

    // Task Creation
    xTaskCreate(Task_Gerencia, "Task_Gerencia", 4000, NULL, 1, NULL);
    xTaskCreate(Task_GPS, "Task_GPS", 4000, NULL, 1, NULL);

    mqtt.setServer(broker, 1883);
}

void loop() {
    if (gps_run && carro_parado) {
        if (WiFi.status() != WL_CONNECTED) {
            cliente_conecta_wifi(false);
        } else if (!mqttWifi.connected()) {
            mqttwifi_conecta();
        }
    } else {
        if (millis() - t_loop > 10000) {
            t_loop = millis();

            // Communication Management
            if ((!modem.isGprsConnected()) && gsm_habilitado) {
                digitalWrite(MODEM_PWKEY, HIGH);
                digitalWrite(MODEM_RST, LOW);
                digitalWrite(MODEM_POWER_ON, LOW);
                delay(1000);
                digitalWrite(MODEM_PWKEY, LOW);
                digitalWrite(MODEM_RST, HIGH);
                digitalWrite(MODEM_POWER_ON, HIGH);
                gsm_conecta();
            } else if (!mqtt.connected() && modem.isGprsConnected()) {
                mqtt_conecta();
            }
            scan_envia_log();
        }

        // MQTT Scan
        if (mqtt.connected()) {
            mqtt.loop();
        }
    }
    vTaskDelay(200 / portTICK_PERIOD_MS);
}

void Task_Gerencia(void *pvParameters) {
    (void) pvParameters;

    while (1) {
        scan_mpu();
        scan_dht22();
        scan_novo_log();

        vTaskDelay(200 / portTICK_PERIOD_MS);
    }
}
