#include <ArduinoJson.h>

#include <Adafruit_NeoPixel.h>

#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiClient.h>

const char* ssid = "orestes_LTE";
const char* password = "hell0wifi";
const int pin = 2;

bool on = true;
int i = 127;
int r = 255;
int g = 0;
int b = 0;

ESP8266WebServer server(80);

// Parameter 1 = number of pixels in strip
// Parameter 2 = Arduino pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(1, pin, NEO_GRB + NEO_KHZ800);

// IMPORTANT: To reduce NeoPixel burnout risk, add 1000 uF capacitor across
// pixel power leads, add 300 - 500 Ohm resistor on first pixel's data input
// and minimize distance between Arduino and first pixel.  Avoid connecting
// on a live circuit...if you must, connect GND first.

void setup(void) {
  Serial.begin(115200);
  
  strip.begin();
  strip.setBrightness(150);
  strip.show(); // Initialize all pixels to 'off'
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin("demo")) {
    Serial.println("MDNS responder started");
  }

  server.on("/state", HTTP_POST, setState);

  server.begin();
  Serial.println("HTTP server started");
}


void loop(void) {
  server.handleClient();
  MDNS.update();

  if (on) {
    strip.setBrightness(i);
    strip.setPixelColor(0, strip.Color(r, g, b));
  } else {
    strip.setBrightness(0);
    strip.setPixelColor(0, strip.Color(0, 0, 0));  
  }
  
  strip.show();
}


void setState() {
  Serial.println("Incoming state: " + server.arg("plain"));
  StaticJsonDocument<256> input;
  deserializeJson(input, server.arg("plain"));

  if (!input["on"].isNull()) {
    on = input["on"];
    if (input["on"]) {
      Serial.println("ON");
    } else {
      Serial.println("OFF");
    }
  }

  if (!input["r"].isNull()) {
    r = constrain(input["r"], 0, 255);
    Serial.println("R = " + r);
  }

  if (!input["g"].isNull()) {
    g = constrain(input["g"], 0, 255);
    Serial.println("G = " + g);
  }

  if (!input["b"].isNull()) {
    b = constrain(input["b"], 0, 255);
    Serial.println("B = " + b);
  }

  if (!input["i"].isNull()) {
    i = constrain(input["i"], 0, 255);
    Serial.println("Brightness = " + i);
  }
  
  server.send(200, "application/json", "{\"success\": true}");
}

