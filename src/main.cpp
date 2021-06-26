#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <ModbusMaster.h>
#include "renogy_rtu.h"
#include <map>

const int ON = LOW;
const int OFF = HIGH;

// Network Config
const char* ssid = "wifi_ap";
const char* password = "password";
const char* mqtt_server = "192.168.0.106";

WiFiClient espClient;
PubSubClient client(espClient);
ModbusMaster node;

// Modbus response buffer
std::map<std::string, uint16_t> response_buffer;

// Solar controller registers list
int counter = 0;
bool msgSwitch = true;
std::vector<uint16_t> registerList;
const PROGMEM char* CURRENT_SOLAR_TOPIC = "/home/backyard/solar_current";
const PROGMEM char*   DAILY_SOLAR_TOPIC = "/home/backyard/solar_daily";
const char* SOLAR_TOPIC = CURRENT_SOLAR_TOPIC;

void loadList(bool currentData)
{
  registerList.clear();

  if(currentData)
  {
    SOLAR_TOPIC = CURRENT_SOLAR_TOPIC;
    registerList.push_back(ReadRegisters::BATT_CAPACITY   );
    registerList.push_back(ReadRegisters::BATT_CAPACITY_AH);
    registerList.push_back(ReadRegisters::BATT_VOLTAGE    );
    registerList.push_back(ReadRegisters::CHARGE_CURRENT  );
    registerList.push_back(ReadRegisters::TEMPERATURE     );
    registerList.push_back(ReadRegisters::LOAD_VOLTAGE    );
    registerList.push_back(ReadRegisters::LOAD_CURRENT    );
    registerList.push_back(ReadRegisters::LOAD_POWER      );
    registerList.push_back(ReadRegisters::PANEL_VOLTAGE   );
    registerList.push_back(ReadRegisters::PANEL_CURRENT   );
    registerList.push_back(ReadRegisters::CHARGE_POWER    );
  }
  else // Daily data
  {
    msgSwitch = true;
    SOLAR_TOPIC = DAILY_SOLAR_TOPIC;
    registerList.push_back(ReadRegisters::BATT_MIN_VOLT   );
    registerList.push_back(ReadRegisters::BATT_MAX_VOLT   );
    registerList.push_back(ReadRegisters::CHARGE_MAX_A    );
    registerList.push_back(ReadRegisters::DISCHARGE_MAX_A );
    registerList.push_back(ReadRegisters::CHARGE_MAX_W    );
    registerList.push_back(ReadRegisters::DISCHARGE_MAX_W );
    registerList.push_back(ReadRegisters::CHARGE_MAX_AH   );
    registerList.push_back(ReadRegisters::DISCHARGE_MAX_AH);
  }
  

}

void wifiSetup()
{
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  int counter = 0;
  int result = 0;

  while (WiFi.status() != WL_CONNECTED) {
    counter++;
    delay(500);
    Serial.print(".");
    if(counter > 20){break; result=-1;}
  }

  randomSeed(micros());

  if(result == 0)
  {
    Serial.println("\nWiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
  }
}

void reconnect() 
{
  // Loop until we're reconnected
  int failure_cnt = 0;
  while (!client.connected()) 
  {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("outTopic", "hello world");
      // ... and resubscribe
      client.subscribe("inTopic");
    } else {
      failure_cnt++;
      Serial.print("failed, rc=");
      Serial.print(client.state());
      if(failure_cnt == 10)
      {
        ESP.restart();
      }
      wifiSetup();
      Serial.println(" try again in 1 second");
      // Wait 1 seconds before retrying
      delay(1000);
    }
  }
}

void publishData(const std::map<std::string, uint16_t>& message)
{
  DynamicJsonDocument doc(1024);

  for(const auto& key : message)
  {
    std::string name = key.first;
    //Serial.println(name.c_str());
    doc[name.c_str()].set(key.second);
  }

  doc.shrinkToFit();
  char payloadData[measureJson(doc)+1];
  serializeJson(doc, payloadData, sizeof(payloadData));

  bool result = client.publish(SOLAR_TOPIC, payloadData, true);
  //Serial.println(result);
  yield();
}

void callback(char* topic, byte* payload, unsigned int length) 
{
  // Serial.print("Message arrived [");
  // Serial.print(topic);
  // Serial.print("] ");
  // for (int i = 0; i < length; i++) {
  //   Serial.print((char)payload[i]);
  // }
  // Serial.println();

  // Switch on the LED if an 1 was received as first character
  if ((char)payload[0] == '1') {
    digitalWrite(BUILTIN_LED, LOW);   // Turn the LED on (Note that LOW is the voltage level
    // but actually the LED is on; this is because
    // it is active low on the ESP-01)
  } else {
    digitalWrite(BUILTIN_LED, HIGH);  // Turn the LED off by making the voltage HIGH
  }

}

void setup() 
{
  Serial.begin(9600);
  Serial.swap();

  pinMode(BUILTIN_LED, OUTPUT);
  digitalWrite(BUILTIN_LED, OFF);
  ESP.wdtDisable(); // use HW watchdog
  wifiSetup();
  ESP.wdtFeed();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  node.begin(1, Serial);
}

uint16_t readSingleRegister(uint16_t registerVal)
{
  uint16_t rv;

  // Read modbus register
  uint8_t result = node.readHoldingRegisters(registerVal, 1);
  if(result == node.ku8MBSuccess)
  {
    rv = node.getResponseBuffer(0);
  }
  else
  {
    rv = result;
  }

  return rv;
}

void loop() 
{
  if (!client.connected()) 
  {
    reconnect();
  }
  client.loop();
  ESP.wdtFeed();

  // every 60 seconds send min/max message
  if(counter == 30)
  {
    msgSwitch = false;
    counter = 0;
  }

  // Build solar controller MQTT message
  loadList(msgSwitch);
  response_buffer.clear();
  for(uint16_t& registerVal : registerList) 
  {
    response_buffer.insert(std::pair<std::string, uint16_t>(Utilities::RegisterToStr(registerVal), readSingleRegister(registerVal)));
    ESP.wdtFeed();
    client.loop();
  }
  
  // Publish data
  publishData(response_buffer);
  node.clearResponseBuffer();
  ESP.wdtFeed();
  delay(2000);
  counter++;
}