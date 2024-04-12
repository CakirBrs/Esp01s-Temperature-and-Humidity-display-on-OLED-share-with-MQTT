#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#define wifi_ssid "id"
#define wifi_password "password"
#define mqtt_server "mqtt server address"
#define mqtt_port 1883

WiFiClient espClient;
PubSubClient client;


#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>
#include <Adafruit_AHTX0.h>
Adafruit_AHTX0 aht;
#include <SSD1306.h>
#define SCREEN_ADDRESS 0x3C 
SSD1306 display(SCREEN_ADDRESS, 0, 2);


void printToOLED(float temp, float humid);
void setupAHT();
void setupOLED();

void reconnect();

void setup() {
  setup_wifi();
  client.setClient(espClient);
  client.setServer(mqtt_server, mqtt_port);
  reconnect(); //MQTT connection
  client.publish("device/Warning", "ESP01 Connected",true);
  
  setupAHT();
  setupOLED();

  
}




void setup_wifi() {
  WiFi.begin(wifi_ssid, wifi_password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    // Attempt to connect
    if (client.connect("ESP8266Client")) {
      delay(100);
    } else {
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}






void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  sensors_event_t humidity, temp;
  aht.getEvent(&humidity, &temp);
  
  printToOLED(temp.temperature,humidity.relative_humidity);
  client.publish("device/Temp", String(temp.temperature).c_str(), true); //sending temperature value to mqtt protocol
  client.publish("device/Humid", String(humidity.relative_humidity).c_str(), true); //sending humidity value to mqtt protocol
  delay(60000); //wait 60 second 
}



void setupAHT(){
  aht.begin();
}

void setupOLED(){
  Wire.pins(0, 2);  // Start the OLED with GPIO 0 and 2 on ESP-01 first one is CSL second is SDA
  Wire.begin(0, 2);
  display.init();
  display.flipScreenVertically();  
  display.clear();
  display.drawString(0, 10, "Waiting Data...");
  display.display();
}

void printToOLED(float temp, float humid){
  display.clear();
  display.drawString(0, 10, "Temp:  "+String(temp));
  display.drawString(0, 30, "Humid: "+String(humid));
  display.display();
}




