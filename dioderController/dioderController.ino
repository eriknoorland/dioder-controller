#include <SPI.h>
#include <UIPEthernet.h>
#include <PubSubClient.h>

// Ethernet settings
byte mac[] = { 0x22, 0xCC, 0xDD, 0xEE, 0xEE, 0x01 };
// IPAddress ip(192, 168, 1, 179);
EthernetServer server(80);
EthernetClient ethernetClient;

// MQTT settings
IPAddress broker(192,168,178,29);
PubSubClient client(ethernetClient);

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

  for (int i = 0; i < length; i++) {
    Serial.print(payload[i]);
  }

  // if (strcmp(topic, "light") == 0) {
  //   if (payload[0] == '1') {
  //     digitalWrite(OUTPUT_PIN_1, HIGH);
  //     Serial.println("Relais on");
  //   } else {
  //     digitalWrite(OUTPUT_PIN_1, LOW);
  //     Serial.println("Relais off");
  //   }
  // }

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
