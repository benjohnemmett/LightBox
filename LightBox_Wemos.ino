#include <Arduino_JSON.h>
#include <Adafruit_NeoPixel.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include "LightBoxWebPage.h"
#include "personal.h"
#include "LedStripManager.h"

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
#define LED_HALF_COUNT  LED_COUNT >> 1

Adafruit_NeoPixel front_strip(LED_COUNT, FRONT_LED_PIN, NEO_GRB);
Adafruit_NeoPixel back_strip(LED_COUNT, BACK_LED_PIN, NEO_GRB);
LedStripManager *front_strip_manager;
LedStripManager *back_strip_manager;

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
  if (front_data.hasOwnProperty("pulse_step")) {
    uint8_t pulse_step_f = (uint8_t) front_data["pulse_step"];
    Serial.print("pulse_step_f: ");
    Serial.println(pulse_step_f);
    front_strip_manager->set_pulse_step(pulse_step_f);
  }
  if (any_colors)
  {
    front_strip_manager->set_color(red, green, blue);
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
  if (back_data.hasOwnProperty("pulse_step")) {
    uint8_t pulse_step_b = (uint8_t) back_data["pulse_step"];
    Serial.print("pulse_step_b: ");
    Serial.println(pulse_step_b);
    back_strip_manager->set_pulse_step(pulse_step_b);
  }
  if (any_colors) {
    back_strip_manager->set_color(red, green, blue); 
  }
}

/////////////////////////////////////////////
// Main Setup & Loop Functions
/////////////////////////////////////////////
void setup(void) {
  Serial.begin(115200);

  WiFi.mode(WIFI_STA);
  WiFi.setHostname("lightbox");
  WiFi.begin(ssid, password);
  Serial.println("");

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
  front_strip_manager = new LedStripManager(front_strip);
  back_strip_manager = new LedStripManager(back_strip);

  front_strip_manager->initialize();
  back_strip_manager->initialize();

  back_strip_manager->set_color(50, 50, 10);
  front_strip_manager->set_color(100, 50, 10);

  front_strip_manager->set_pulse_step(1);
  back_strip_manager->set_pulse_step(2);
}

void loop(void) {
  server.handleClient(); 
  MDNS.update();

  front_strip_manager->update();
  back_strip_manager->update();
}
