#include <DHTesp.h>
#include "Ticker.h"
#include <WiFi.h>
#include <PubSubClient.h>
const char* ssid = "name";//Network name to be connected 
const char* password = "pass";// Network password 
const char* mqtt_server = "ip of raspberry Pi";//local or remote ip of the MQTT server without port 

#ifndef ESP32
#pragma message(THIS EXAMPLE IS FOR ESP32 ONLY!)
#error Select ESP32 board.
#endif
DHTesp::DHT_MODEL_t DHT_TYPE = DHTesp::AM2302;

WiFiClient espClient;
PubSubClient client(espClient);
DHTesp dht;
long lastMsg = 0;
char msg[50];
int value = 0;

//orismos max6675 watertemperature
#include <Thermocouple.h>
#include <MAX6675_Thermocouple.h>

#define SCK_PIN 18
#define CS_PIN 5
#define SO_PIN 19

Thermocouple* thermocouple;

int enable2 = 25;      // enable reading Rain sensor

void tempTask(void *pvParameters);
bool getTemperature();
void triggerGetTemp();

/** Task handle for the light value read task */
TaskHandle_t tempTaskHandle = NULL;
/** Ticker for temperature reading */
Ticker tempTicker;
/** Comfort profile */
ComfortState cf;
/** Flag if task should run */
bool tasksEnabled = false;
/** Pin number for DHT11 data pin */
const int DHT_PIN = 26;

//photoresistors
const int kithcenPhotoresistor = 36;
const int livingroomPhotoresistor = 39;
const int bedroomPhotoresistor = 33;
const int garagePhotoresistor = 32;
const int bothroomPhotoresistor = 34;
const int roofPhotoresistor = 35;

int PhotoresistorsVal[6] = {0, 0, 0, 0, 0, 0};
int lightVal;   // light reading

void readPhoto(int  PhotoresistorsVal[]) {

  PhotoresistorsVal[0] = analogRead(kithcenPhotoresistor);
  delay (100);
  PhotoresistorsVal[1] = analogRead(livingroomPhotoresistor);
  delay (100);
  PhotoresistorsVal[2] = analogRead(bedroomPhotoresistor);
  delay (100);
  PhotoresistorsVal[3] = analogRead(garagePhotoresistor);
  delay (100);
  PhotoresistorsVal[4] = analogRead(bothroomPhotoresistor);
  delay (100);
  PhotoresistorsVal[5] = analogRead(roofPhotoresistor);
  delay(100);
}
// telos photoresistor

bool initTemp() {
  byte resultValue = 0;
  // Initialize temperature sensor
  dht.setup(DHT_PIN, DHTesp::AM2302);
  Serial.println("DHT initiated");

  // Start task to get temperature
  xTaskCreatePinnedToCore(
    tempTask,                       /* Function to implement the task */
    "tempTask ",                    /* Name of the task */
    4000,                           /* Stack size in words */
    NULL,                           /* Task input parameter */
    5,                              /* Priority of the task */
    &tempTaskHandle,                /* Task handle. */
    1);                             /* Core where the task should run */

  if (tempTaskHandle == NULL) {
    Serial.println("Failed to start task for temperature update");

    return false;
  } else {
    // Start update of environment data every 20 seconds
    tempTicker.attach(20, triggerGetTemp);
  }
  return true;
}

void triggerGetTemp() {
  if (tempTaskHandle != NULL) {
    xTaskResumeFromISR(tempTaskHandle);
  }
}

void tempTask(void *pvParameters) {
  Serial.println("tempTask loop started");
  while (1) // tempTask loop
  {
    if (tasksEnabled) {
      // Get temperature values
      getTemperature();
    }
    // Got sleep again
    vTaskSuspend(NULL);
  }
}

/**
   getTemperature
   Reads temperature from DHT11 sensor
   @return bool
      true if temperature could be aquired
      false if aquisition failed
*/
void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
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

void callback(char* topic, byte* message, unsigned int length) {// this function checks retreived messages 
  Serial.print(topic);
  Serial.print(". Message: ");
  String messageTemp;

  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println();

  // If a message is received on the topic esp32/output, you check if the message is either "on" or "off".
  // Changes the output state according to the message
  if (String(topic) == "esp32/output") {
    Serial.print("Changing output to ");
    if (messageTemp == "on") {
      Serial.println("on");

    }
    else if (messageTemp == "off") {
      Serial.println("off");
    }
  }
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP8266Client")) {
      Serial.println("connected");
      // Subscribe to MQTT topic 
      client.subscribe("esp32/output");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup()
{
  Serial.begin(115200);
  Serial.println();
  Serial.println("DHT ESP32 example with tasks");
  setup_wifi();//WiFi connection
  client.setServer(mqtt_server, 1883);//MQTT establish connection with the server 
  client.setCallback(callback);
  initTemp();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  // Signal end of setup() to tasks
  tasksEnabled = true;

  thermocouple = new MAX6675_Thermocouple(SCK_PIN, CS_PIN, SO_PIN);
  pinMode(enable2, INPUT);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  TempAndHumidity newValues = dht.getTempAndHumidity();
  if (!tasksEnabled) {
    // Wait 2 seconds to let system settle down
    delay(8000);
    // Enable task that will read values from the DHT sensor
    tasksEnabled = true;
    if (tempTaskHandle != NULL) {
      vTaskResume(tempTaskHandle);
    }
  }
  String values = getTemperature2();

  char tempString[8];
  dtostrf(newValues.temperature, 1, 2, tempString);
  Serial.print("Temperature: ");
  Serial.println(tempString);
  client.publish("esp32/temperature", tempString);

  char humString[8];
  dtostrf(newValues.humidity, 1, 2, humString);
  Serial.print("Humidity: ");
  Serial.println(humString);
  client.publish("esp32/humidity", humString);

  //publish photoresistors through MQTT

  readPhoto( PhotoresistorsVal);
  String garage = String(PhotoresistorsVal[3]);
  char gs[8];
  Serial.println("Photoresistor 1");
  Serial.println(PhotoresistorsVal[0]);
  Serial.println("Photoresistor 2");
  Serial.println(PhotoresistorsVal[1]);
  Serial.println("Photoresistor 3");
  Serial.println(PhotoresistorsVal[2]);
  Serial.println("Photoresistor 4");
  Serial.println(PhotoresistorsVal[3]);
  Serial.println("Photoresistor 5");
  Serial.println(PhotoresistorsVal[4]);
  Serial.println("Photoresistor 6");
  Serial.println(PhotoresistorsVal[5]);

  dtostrf(PhotoresistorsVal[0], 1, 2, gs);
  client.publish("esp32/kitchenlight", gs);
  dtostrf(PhotoresistorsVal[1], 1, 2, gs);
  client.publish("esp32/livinglight", gs);
  dtostrf(PhotoresistorsVal[2], 1, 2, gs);
  client.publish("esp32/bedroomlight", gs);
  dtostrf(PhotoresistorsVal[3], 1, 2, gs);
  client.publish("esp32/garagelight", gs);
  dtostrf(PhotoresistorsVal[4], 1, 2, gs);
  client.publish("esp32/bathroomlight", gs);
  dtostrf(PhotoresistorsVal[5], 1, 2, gs);
  client.publish("esp32/outdoorlight", gs);

  const double celsius = thermocouple->readCelsius();
  dtostrf(celsius, 1, 2, gs);
  client.publish("esp32/watertemperature", gs);

  if (digitalRead(enable2) == HIGH) {
    Serial.println("no rain");
    client.publish("esp32/RainSensor", "false");

  }
  else {
    Serial.println(" rain");
    client.publish("esp32/RainSensor", "true");
  }


  delay(10000);

  yield();
}
