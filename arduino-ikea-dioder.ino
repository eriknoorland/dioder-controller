#include <UIPEthernet.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <Ramp.h>

#define PIN_RED 5
#define PIN_GREEN 6
#define PIN_BLUE 3

#define TOPIC_PUBLISH "pc_led_debug"
#define TOPIC_SUBSCRIBE "pc_led"

byte mac[] = { 0x22, 0xCC, 0xDD, 0xEE, 0xEE, 0x02 };
EthernetServer server(80);
EthernetClient ethernetClient;

IPAddress broker(192, 168, 1, 2);
PubSubClient client(ethernetClient);

rampInt redRamp;
rampInt greenRamp;
rampInt blueRamp;

/**
 * MQTT callback
 * @param {char} topic
 * @param {byte} payload
 * @param {uint} length
 */
void callback(char* topic, byte* payload, unsigned int length) {
  if (strcmp(topic, TOPIC_SUBSCRIBE) == 0) {
    const size_t capacity = JSON_ARRAY_SIZE(3) + JSON_OBJECT_SIZE(1) + 20;
    DynamicJsonBuffer jsonBuffer(capacity);
    JsonObject& root = jsonBuffer.parseObject(payload);
    JsonArray& colour = root["colour"];

    redRamp.go(colour[0], 500, LINEAR, ONCEFORWARD);
    greenRamp.go(colour[1], 500, LINEAR, ONCEFORWARD);
    blueRamp.go(colour[2], 500, LINEAR, ONCEFORWARD);

    client.publish(TOPIC_PUBLISH, "Message received!");
  }
}

/**
 * MQTT reconnect
 */
void reconnect() {
  while (!client.connected()) {
    if (client.connect(TOPIC_SUBSCRIBE)) {
      client.publish(TOPIC_PUBLISH, "Connected!");
      client.subscribe(TOPIC_SUBSCRIBE);
    } else {
      delay(5000);
    }
  }
}

/**
 * Setup
 */
void setup() {
  pinMode(PIN_RED, OUTPUT);
  pinMode(PIN_GREEN, OUTPUT);
  pinMode(PIN_BLUE, OUTPUT);

  client.setServer(broker, 1881);
  client.setCallback(callback);

  Ethernet.begin(mac);
  server.begin();
}

/**
 * Loop
 */
void loop() {
  EthernetClient ethernetClient = server.available();

  if (!client.connected()) {
    reconnect();
    return;
  }

  client.loop();

  if (redRamp.isRunning()) {
    analogWrite(PIN_RED, redRamp.update());
  }

  if (greenRamp.isRunning()) {
    analogWrite(PIN_GREEN, greenRamp.update());
  }

  if (blueRamp.isRunning()) {
    analogWrite(PIN_BLUE, blueRamp.update());
  }
}
