#include <Arduino_JSON.h>
#include <Adafruit_NeoPixel.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include "LightBoxWebPage.h"
#include "personal.h"

#ifndef STASSID
#define STASSID "wifi-name"
#define STAPSK  "wifi-password"
#endif

/////////////////////////////////////////////
// LED Stuff
/////////////////////////////////////////////
#define FRONT_LED_PIN   D4
#define BACK_LED_PIN    D1
#define LED_COUNT       22

Adafruit_NeoPixel front_strip(LED_COUNT, FRONT_LED_PIN, NEO_GRB);
Adafruit_NeoPixel back_strip(LED_COUNT, BACK_LED_PIN, NEO_GRB);

uint32_t c1 = front_strip.Color(50,   100,   100);
uint32_t c2 = front_strip.Color(50,   10,   200);
uint32_t c3 = front_strip.Color(10,   200,   50);
uint32_t* color_f;
uint32_t* color_b;
uint8_t light_index_f = 0;
uint8_t light_index_b = 0;

/////////////////////////////////////////////
// Web Stuff
/////////////////////////////////////////////
const char* ssid = STASSID;
const char* password = STAPSK;

ESP8266WebServer server(80);

void handleRoot() {
  server.send(200, "text/html", light_box_html);
}

void handleNotFound() {
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}

void handleJson() {
  uint8_t red, green, blue;
  Serial.println("");
  Serial.println("handleJson: ");

  //https://github.com/esp8266/Arduino/issues/1321
  String json(server.arg("plain"));
  Serial.print("json: ");
  Serial.println(json);
  JSONVar myObject = JSON.parse(json);

  if (JSON.typeof(myObject) == "undefined") {
    Serial.println("Parsing json input failed!");
    return;
  }

  if (!myObject.hasOwnProperty("front_data")) {
    Serial.println("Missing json front data!");
    return;
  }

  if (!myObject.hasOwnProperty("back_data")) {
    Serial.println("Missing json front data!");
    return;
  }

  JSONVar front_data = myObject["front_data"];
  JSONVar back_data = myObject["back_data"];

  uint8_t any_colors = 0;
  if (front_data.hasOwnProperty("red")) {
    red = (uint8_t) front_data["red"];
    any_colors = 1;
    Serial.print("red: ");
    Serial.println(red);
  }
  if (front_data.hasOwnProperty("green")) {
    green = (uint8_t) front_data["green"];
    any_colors = 1;
    Serial.print("green: ");
    Serial.println(green);
  }
  if (front_data.hasOwnProperty("blue")) {
    blue = (uint8_t) front_data["blue"];
    any_colors = 1;
    Serial.print("blue: ");
    Serial.println(blue);
  } 
  if (any_colors)
  {
    c1 = front_strip.Color(red, green, blue); 
    color_f = &c1;
    light_index_f = 0;
  }

  any_colors = 0;
  if (back_data.hasOwnProperty("red")) {
    red = (uint8_t) back_data["red"];
    any_colors = 1;
    Serial.print("red: ");
    Serial.println(red);
  }
  if (back_data.hasOwnProperty("green")) {
    green = (uint8_t) back_data["green"];
    any_colors = 1;
    Serial.print("green: ");
    Serial.println(green);
  }
  if (back_data.hasOwnProperty("blue")) {
    blue = (uint8_t) back_data["blue"];
    any_colors = 1;
    Serial.print("blue: ");
    Serial.println(blue);
  } 
  if (any_colors) {
    c2 = front_strip.Color(red, green, blue); 
    color_b = &c2;
    light_index_b = 0;
  }
}

/////////////////////////////////////////////
// LED Functions
/////////////////////////////////////////////
void init_led_strip(Adafruit_NeoPixel* led_strip)
{
  led_strip->begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
  led_strip->show();            // Turn OFF all pixels ASAP
  led_strip->setBrightness(30); // Set BRIGHTNESS to about 1/5 (max = 255)
}


/////////////////////////////////////////////
// Main Setup & Loop Functions
/////////////////////////////////////////////
void setup(void) {
  Serial.begin(115200);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");

  color_f = &c1;
  color_b = &c2;

  // Wait for connection
  Serial.print("Connecting to ");
  Serial.println(ssid);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin("esp8266")) {
    Serial.println("MDNS responder started");
  }

  server.on("/", handleRoot);
  server.on("/json", HTTP_POST, handleJson);
  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");

  //////////////////////////////////////////////////
  // LED Stuff
  //////////////////////////////////////////////////
  init_led_strip(&front_strip);
  init_led_strip(&back_strip);
}

void loop(void) {
  server.handleClient(); 
  MDNS.update();

  bool changed_a_light = false;
  if (light_index_f < LED_COUNT)
  {
    changed_a_light = true;
    light_index_f = light_index_f + 1;
    front_strip.setPixelColor(light_index_f, *color_f);
    front_strip.show();
  } 

  if (light_index_b < LED_COUNT)
  {
    changed_a_light = true;
    light_index_b = light_index_b + 1;
    back_strip.setPixelColor(light_index_b, *color_b);
    back_strip.show();
  } 

  if (changed_a_light) {
    delay(50);
  }
}
