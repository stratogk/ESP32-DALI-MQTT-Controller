#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>



const char* ssid = "-----";
const char* password = "-----";

const char* mqtt_server = "192.168.xxx.xxx";
const char* mqtt_user = "mqtt"; 
const char* mqtt_pass = "test";

const char* DEVICE_NAME  = "My DALI Controller"; 
const char* DEVICE_ID    = "esp32_dali_master_v3"; 
const char* MANUFACTURER = "My Smart Home";
const char* MODEL        = "ESP32-S3 Pro Controller";
const char* MQTT_PREFIX  = "dali/v3";
// --- DALI PINS ---
const int DALI_TX_PIN = 21; 
const int DALI_RX_PIN = 40;

const int TE = 416;

portMUX_TYPE myMutex = portMUX_INITIALIZER_UNLOCKED; 
unsigned long lastBriTime = 0; 

WiFiClient espClient;
PubSubClient client(espClient);

// --- DALI FUNCTIONS ---
void setupDALI() {
  pinMode(DALI_TX_PIN, OUTPUT);
  digitalWrite(DALI_TX_PIN, LOW); 
}

void sendBit(bool b) {
  if (b) {
    digitalWrite(DALI_TX_PIN, HIGH); delayMicroseconds(TE);
    digitalWrite(DALI_TX_PIN, LOW);  delayMicroseconds(TE);
  } else {
    digitalWrite(DALI_TX_PIN, LOW);  delayMicroseconds(TE);
    digitalWrite(DALI_TX_PIN, HIGH); delayMicroseconds(TE);
  }
}

void sendDaliCmd(byte addressByte, byte commandByte) {
  portENTER_CRITICAL(&myMutex); 
  sendBit(true); 
  for (int i = 7; i >= 0; i--) sendBit((addressByte >> i) & 1);
  for (int i = 7; i >= 0; i--) sendBit((commandByte >> i) & 1);
  digitalWrite(DALI_TX_PIN, LOW); 
  portEXIT_CRITICAL(&myMutex); 
  delay(10); 
}

void setDaliLevel(int shortAddr, int level) {
  byte addrByte = (shortAddr << 1); 
  if (level < 0) level = 0;     
  if (level > 254) level = 254; 
  
  sendDaliCmd(addrByte, (byte)level);
  delay(5);
  sendDaliCmd(addrByte, (byte)level);
}

// --- DISCOVERY ---
void sendDiscovery(int lampId) {
  // Discovery Topic (Standard Home Assistant Path)
  String discoveryTopic = "homeassistant/light/" + String(DEVICE_ID) + "_" + String(lampId) + "/config";
  
  DynamicJsonDocument doc(1024);
  
  doc["name"] = String(DEVICE_NAME) + " Lamp " + String(lampId);
  doc["unique_id"] = String(DEVICE_ID) + "_light_" + String(lampId);
  
  JsonObject device = doc.createNestedObject("device");
  device["identifiers"] = DEVICE_ID;   
  device["name"] = DEVICE_NAME;       
  device["manufacturer"] = MANUFACTURER; 
  device["model"] = MODEL;               


  String baseTopic = String(MQTT_PREFIX) + "/light/" + String(lampId);
  
  doc["command_topic"] = baseTopic + "/set";
  doc["payload_on"] = "ON";
  doc["payload_off"] = "OFF";
  doc["brightness_command_topic"] = baseTopic + "/bri";
  doc["brightness_scale"] = 254;
  doc["optimistic"] = true;

  char buffer[512];
  serializeJson(doc, buffer);
  client.publish(discoveryTopic.c_str(), buffer, true);
  
  client.subscribe((baseTopic + "/set").c_str());
  client.subscribe((baseTopic + "/bri").c_str());
  delay(100);
}

// --- CALLBACK ---
void callback(char* topic, byte* payload, unsigned int length) {
  String message = "";
  for (unsigned int i = 0; i < length; i++) message += (char)payload[i];
  message.trim(); 

  String topicStr = String(topic);
  

  String searchBase = String(MQTT_PREFIX) + "/light/";

  if (topicStr.indexOf(searchBase) >= 0) {

      int idIndex = topicStr.indexOf("light/") + 6;
      int slashIndex = topicStr.indexOf("/", idIndex);
      
      String idStr = topicStr.substring(idIndex, slashIndex);
      int lampId = idStr.toInt();
      String cmdType = topicStr.substring(slashIndex + 1);

      if (cmdType == "bri") {
        int brightness = message.toInt();
        setDaliLevel(lampId, brightness);
        lastBriTime = millis(); 
      }
      else if (cmdType == "set") {
        if (message == "ON") {
          unsigned long diff = millis() - lastBriTime;
          if (diff > 2000) { 
             setDaliLevel(lampId, 254);
          }
        }
        else if (message == "OFF") {
          setDaliLevel(lampId, 0);
        }
      }
  }
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Connecting MQTT...");
    String clientId = String(DEVICE_ID) + "-" + WiFi.macAddress();
    
    if (client.connect(clientId.c_str(), mqtt_user, mqtt_pass)) {
      Serial.println("OK");
      sendDiscovery(1);
      sendDiscovery(2); 
    } else {
      Serial.print("fail rc=");
      Serial.print(client.state());
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  setupDALI();
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) delay(500);
  
  client.setBufferSize(1024); 
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) reconnect();
  client.loop();
}