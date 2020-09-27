#include <UIPEthernet.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

// Ethernet settings
byte mac[] = { 0x22, 0xCC, 0xDD, 0xEE, 0xEE, 0x02 };
EthernetServer server(80);
EthernetClient ethernetClient;

// MQTT settings
IPAddress broker(192, 168, 178, 15);
PubSubClient client(ethernetClient);

bool serialDebug = true;

// LED settings
#define PIN_RED 9
#define PIN_GREEN 6
#define PIN_BLUE 3

/**
 * MQTT callback
 * @param {char} topic
 * @param {byte} payload
 * @param {uint} length
 */
void callback(char* topic, byte* payload, unsigned int length) {
  if (serialDebug) {
    Serial.print("Message arrived [");
    Serial.print(topic);
    Serial.print("] ");
  }

  if (strcmp(topic, "pc_led") == 0) {
    StaticJsonBuffer<200> jsonBuffer;
    JsonObject& root = jsonBuffer.parseObject(payload);
    JsonArray& colour = root["colour"];

    analogWrite(PIN_RED, colour[0]);
    analogWrite(PIN_GREEN, colour[1]);
    analogWrite(PIN_BLUE, colour[2]);

    client.publish("pc_led_debug", "Message received!");
  }

  if (serialDebug) {
    Serial.println();
  }
}

/**
 * MQTT reconnect
 */
void reconnect() {
  while (!client.connected()) {
    if (serialDebug) {
      Serial.print("Attempting MQTT connection...");
    }

    if (client.connect("pc_led")) {
      if (serialDebug) {
        Serial.println("connected");
      }

      client.publish("pc_led_debug", "Connected!");
      client.subscribe("pc_led");
    } else {

      if (serialDebug) {
        Serial.print("failed, rc=");
        Serial.print(client.state());
        Serial.println(" try again in 5 seconds");
      }

      delay(5000);
    }
  }
}

/**
 * Setup
 */
void setup() {
  if (serialDebug) {
    Serial.begin(9600);
  }

  pinMode(PIN_RED, OUTPUT);
  pinMode(PIN_GREEN, OUTPUT);
  pinMode(PIN_BLUE, OUTPUT);

  client.setServer(broker, 1881);
  client.setCallback(callback);

  Ethernet.begin(mac);
  server.begin();

  if (serialDebug) {
    Serial.print("IP Address: ");
    Serial.println(Ethernet.localIP());
  }
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
