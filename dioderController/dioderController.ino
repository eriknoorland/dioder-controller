#include <UIPEthernet.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

// Ethernet settings
byte mac[] = { 0x22, 0xCC, 0xDD, 0xEE, 0xEE, 0x01 };
EthernetServer server(80);
EthernetClient ethernetClient;

// MQTT settings
IPAddress broker(192,168,178,29);
PubSubClient client(ethernetClient);

// LED settings
int PIN_RED = 9;
int PIN_GREEN = 6;
int PIN_BLUE = 3;

/**
 * MQTT callback
 * @param {char} topic
 * @param {byte} payload
 * @param {uint} length
 */
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");

  // for (int i = 0; i < length; i++) {
  //   Serial.print(payload[i]);
  // }

  if (strcmp(topic, "dioder") == 0) {
    StaticJsonBuffer<200> jsonBuffer;
    JsonObject& root = jsonBuffer.parseObject(payload);
    JsonArray& colour = root["colour"];

    int red = colour[0];
    int green = colour[1];
    int blue = colour[2];

    analogWrite(PIN_RED, red);
    analogWrite(PIN_GREEN, green);
    analogWrite(PIN_BLUE, blue);
  }

  Serial.println();
}

/**
 * MQTT reconnect
 */
void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");

    if (client.connect("dioder")) {
      Serial.println("connected");
      client.subscribe("dioder");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

/**
 * Setup
 */
void setup() {
  Serial.begin(9600);

  pinMode(PIN_RED, OUTPUT);
  pinMode(PIN_GREEN, OUTPUT);
  pinMode(PIN_BLUE, OUTPUT);

  client.setServer(broker, 1881);
  client.setCallback(callback);

  Ethernet.begin(mac);
  server.begin();

  Serial.print("IP Address: ");
  Serial.println(Ethernet.localIP());
}

/**
 * Loop
 */
void loop() {
  EthernetClient ethernetClient = server.available();

  if (!client.connected()) {
    reconnect();
  }

  client.loop();
}
