#include <PubSubClient.h>

WiFiClient client_wifi;
PubSubClient mqttWifi(client_wifi);
PubSubClient mqtt(client_gsm);


const char* broker = "broker.hivemq.com";
const char* topico_mqtt = "node/transport/bioma/";



boolean mqtt_conecta();

boolean mqttwifi_conecta();

boolean mqtt_conecta() {
  Serial.print("Conectando ao broker mqtt ");
  Serial.print(broker);


  boolean status = mqtt.connect("espClient");



  if (status == false) {
    Serial.println(" falha ao conectar no broker");
    ESP.restart();
    return false;
  }
  Serial.println(" conectado com sucesso ao broker");
  //  mqtt.subscribe(topicOutput1);
  //  mqtt.subscribe(topicOutput2);

  return mqtt.connected();
}


boolean mqttwifi_conecta() {
  Serial.print("Conectando ao broker mqtt ");
  Serial.print(broker);


  boolean status = mqtt.connect("espClient");



  if (status == false) {
    Serial.println(" falha ao conectar no broker");
    ESP.restart();
    return false;
  }
  Serial.println(" conectado com sucesso ao broker");
  //  mqtt.subscribe(topicOutput1);
  //  mqtt.subscribe(topicOutput2);

  return mqtt.connected();
}
