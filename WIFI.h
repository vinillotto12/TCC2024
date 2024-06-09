//DADOS WIFI
bool wifi_habilitado = false;
char ssid[60] = "NOS-F646";
char password[60] = "5KV9XHJ7";
//char ssid[60] = "agents";
//char password[60] = "QgC9O8VucAByqvVu5Rruv1zdpqM66cd23KG4ElV7vZiJND580bzYvaHqz5k07G2";
char ip[60] = "0";
char gateway[60] = "0";
char subnet[60] = "0";

//ROTINAS
void cliente_conecta_wifi(boolean portal);

void cliente_conecta_wifi(boolean portal) {

  WiFi.disconnect();
  WiFi.setSleep(false);
  WiFi.setAutoReconnect(false);
  WiFi.mode(WIFI_STA);
  delay(100);
  WiFi.begin(ssid, password);

  String aux_ip = (String) ip;

  //IP ESTATICO
  if (!aux_ip.equals("0")) {
    IPAddress ip_get;
    IPAddress gatew_get;
    IPAddress subnet_get;
    IPAddress dns { 8, 8, 8, 8};
    ip_get.fromString(ip);
    gatew_get.fromString(gateway);
    subnet_get.fromString(subnet);
    WiFi.config(ip_get, gatew_get, subnet_get, dns);
    Serial.println("[WIFI] - WIFI Configurado com IP");
  } else {
    Serial.println("[WIFI] - WIFI Configurado via DHCP");

  }

  Serial.print("[WIFI] - SSID ");
  Serial.println(ssid);
  Serial.print("[WIFI] - SENHA ");
  Serial.println(password);
  Serial.println(" ");
  int wait = 0;
  bool segunda_tentativa = false;
  while (WiFi.status() != WL_CONNECTED and wait <= 15)
  {

    if (wait >= 14 && !segunda_tentativa && portal) {
      wait = 0;
      segunda_tentativa = true;
      Serial.println(" ");
      WiFi.disconnect();
      WiFi.setSleep(false);
      WiFi.setAutoReconnect(false);
      WiFi.mode(WIFI_STA);
      delay(100);
      WiFi.begin(ssid, password);
    } else {
      Serial.print(".");
      wait++;
    }

    vTaskDelay( 1000 / portTICK_PERIOD_MS );
  }
  Serial.println(" ");



  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("- DADOS DA REDE WIFI CONECTADO");
    Serial.println(WiFi.localIP());
    Serial.println(WiFi.gatewayIP());
    Serial.println(WiFi.subnetMask());
    delay(100);


  }


}
