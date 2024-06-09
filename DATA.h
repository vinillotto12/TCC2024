#include <Arduino.h>
#include <SPIFFS.h>
#include <ArduinoJson.h>
#include <PubSubClient.h>

// Declaração externa do semáforo para controle de acesso ao sistema de arquivos
extern SemaphoreHandle_t xFlashSemaphore;

// Variáveis globais
String dirArquivo = "null";
const int tempoNovoLogMin = 1; // Intervalo para criar novo log, em minutos
unsigned long ultimoTempoLog = millis();

// Protótipos de Funções
void atualizarDiretorioArquivo();
String lerArquivo();
bool apagarArquivo(const String& caminho);
void criarDiretorio(const String& caminho);
void listarDiretorio(const char* nomeDiretorio, uint8_t niveis);
void processarNovoLog();
void processarEnvioLog();

// Implementações das Funções
void atualizarDiretorioArquivo() {
    File root = SPIFFS.open("/enviar");
    if (!root) {
        Serial.println("- Nenhum arquivo encontrado");
        dirArquivo = "null";
        return;
    }
    if (!root.isDirectory()) {
        Serial.println(" - Diretorio nao existe");
        dirArquivo = "null";
        return;
    }

    File file = root.openNextFile();
    if (file) {
        dirArquivo = file.name();
        file.close();
    } else {
        dirArquivo = "null";
    }
    root.close();
}

String lerArquivo() {
    String conteudo = "";
    if (dirArquivo == "null") {
        atualizarDiretorioArquivo();
    }
    File configFile = SPIFFS.open(dirArquivo, "r");
    if (configFile) {
        while (configFile.available()) {
            conteudo += configFile.readStringUntil('\n');
        }
        configFile.close();
    }
    return conteudo;
}

bool apagarArquivo(const String& caminho) {
    if (SPIFFS.remove(caminho)) {
        Serial.println(" - Arquivo apagado: " + caminho);
        return true;
    } else {
        Serial.println(" - Erro ao apagar arquivo: " + caminho);
        return false;
    }
}

void criarDiretorio(const String& caminho) {
    if (!SPIFFS.mkdir(caminho)) {
        Serial.println("Erro ao criar o diretorio: " + caminho);
    } else {
        Serial.println("Diretorio criado: " + caminho);
    }
}

void listarDiretorio(const char* nomeDiretorio, uint8_t niveis) {
    File root = SPIFFS.open(nomeDiretorio);
    if (!root || !root.isDirectory()) {
        Serial.println("Falha ao abrir ou nao e um diretorio: " + String(nomeDiretorio));
        return;
    }

    File file = root.openNextFile();
    while (file) {
        if (file.isDirectory()) {
            Serial.print("  DIR : ");
            Serial.println(file.name());
            if (niveis) {
                listarDiretorio(file.name(), niveis - 1);
            }
        } else {
            Serial.print("  FILE: ");
            Serial.print(file.name());
            Serial.print("  SIZE: ");
            Serial.println(file.size());
        }
        file = root.openNextFile();
    }
}

void processarNovoLog() {
    if ((millis() - ultimoTempoLog > (tempoNovoLogMin * 60000))) {
        ultimoTempoLog = millis();
        StaticJsonDocument<256> json;
        
        //logica JSON
        json["DHT"]["temperatura"] = isnan(temperatura) ? 0 : temperatura;
        json["DHT"]["umidade"] = isnan(umidade) ? 0 : umidade;
        json["DHT"]["AlarmTemp"] = alarme_temperatura;
        json["DHT"]["AlarmUmid"] = alarme_umidade;
        json["ACC"]["x"] = escala_accelerometer[0];
        json["ACC"]["y"] = escala_accelerometer[1];
        json["ACC"]["z"] = escala_accelerometer[2];
        json["millis"] = millis();
        //json["hora"] = "";
        
        xSemaphoreTake(xFlashSemaphore, portMAX_DELAY);
        String arquivo = "/enviar/arquivo_" + String(random(1, 100)) + ".json";
        File configFile = SPIFFS.open(arquivo, "w");
        if (!configFile) {
            Serial.println("Falha ao criar arquivo de log");
        } else {
            if (serializeJson(json, configFile) == 0) {
                Serial.println("Falha ao escrever no arquivo de log");
            } else {
                Serial.println("Log salvo: " + arquivo);
            }
            configFile.close();
        }
        xSemaphoreGive(xFlashSemaphore);
    }
}

void processarEnvioLog() {
    if (!dirArquivo.equals("null")) {
        String dadosLog = lerArquivo();
        // Adicione aqui a lógica para enviar os dados do log
        // ...

        if (apagarArquivo(dirArquivo)) {
            dirArquivo = "null";
        }
    }
}

void cria_diretorio(const String& path) {
    if (!SPIFFS.exists(path)) {
        if (SPIFFS.mkdir(path)) {
            Serial.println("Diretorio criado: " + path);
        } else {
            Serial.println("Erro ao criar o diretorio: " + path);
        }
    }
}

void listDir(const char* dirname, uint8_t levels) {
    File root = SPIFFS.open(dirname);
    if (!root) {
        Serial.println("Nenhum arquivo encontrado em " + String(dirname));
        return;
    }

    File file = root.openNextFile();
    while (file) {
        if (file.isDirectory()) {
            Serial.print("DIR : ");
            Serial.println(file.name());
            if (levels) {
                listDir(file.name(), levels - 1);
            }
        } else {
            Serial.print("FILE: ");
            Serial.print(file.name());
            Serial.print(" SIZE: ");
            Serial.println(file.size());
        }
        file = root.openNextFile();
    }
}

