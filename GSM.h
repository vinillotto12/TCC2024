#define TINY_GSM_MODEM_SIM800
#define MODEM_RST            5
#define MODEM_PWKEY          4
#define MODEM_POWER_ON       23
#define MODEM_TX             27
#define MODEM_RX             26


#define SerialAT Serial1
#define GSM_PIN ""

const char apn[] = "zap.vivo.com.br";
const char gprsUser[] = "";
const char gprsPass[] = "";

const char simPIN[]   = "6627";

int gsm_cont_error = 0;

#include <TinyGsmClient.h>

TinyGsm modem(SerialAT);
TinyGsmClient client_gsm(modem);


void gsm_conecta();
void gsm_setup();

void gsm_setup() {
  digitalWrite(MODEM_PWKEY, LOW);
  digitalWrite(MODEM_RST, HIGH);
  digitalWrite(MODEM_POWER_ON, HIGH);
  // Set GSM module baud rate and UART pins
  SerialAT.begin(115200, SERIAL_8N1, MODEM_RX, MODEM_TX);
  gsm_conecta();
}

void gsm_conecta() {


  Serial.println("Conectando ao GPRS...");


  delay(6000);

  // Restart takes quite some time
  // To skip it, call init() instead of restart()
  Serial.println("Inicilizando SIM800...");
  modem.restart();
  //modem.init();

  String modemInfo = modem.getModemInfo();
  Serial.print("Modem Info: ");
  Serial.println(modemInfo);

  if (modemInfo.length() < 2) {
    Serial.println(" falha na conexão com GPRS");
    ESP.restart();
  }

  // Unlock your SIM card with a PIN if needed
  if ( GSM_PIN && modem.getSimStatus() != 3 ) {
    modem.simUnlock(GSM_PIN);
  }

  if (!modem.gprsConnect(apn, gprsUser, gprsPass)) {
    Serial.println(" falha na conexão com GPRS");
    ESP.restart();
  }
  else {
    Serial.println(" OK");
  }

  if (modem.isGprsConnected()) {
    Serial.println("GPRS conectado");
  }
}
