#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#define DOOR 5
#define MQTTlock "house/lock"
#define MQTTdoor "house/door"

//****************************** MQTT Credentials
#define mqtt_server "yourMQTTServer"
#define mqtt_user "yourMQTTUser"
#define mqtt_password "yourMQTTPassword"
#define chip_name "yourChipName"

//****************************** WiFi Credentials
const char* ssid = "yourSSID";
const char* password = "yourPASSWORD";

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;
char message_buff[100];

void setup() {
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  pinMode(DOOR, INPUT_PULLUP);
}

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  int i = 0;
  char message_buff[100];
  String StrPayload;
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (i = 0; i < length; i++) {
    message_buff[i] = payload[i];
  }
  message_buff[i] = '\0';
  StrPayload = String(message_buff);
  int IntPayload = StrPayload.toInt();
  Serial.print(StrPayload);
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect(chip_name, mqtt_user, mqtt_password)) {
      Serial.println("connected");
      client.publish(MQTTlock, "LOCK");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  delay(10);
  client.loop();

  // Checking the door status and reporting to the MQTT server if it is open.
  int doorState = digitalRead(DOOR);
  if(doorState == LOW) { // assuming the door sensor gives LOW when the door is open
    client.publish(MQTTdoor, "OPEN");
  }
}
