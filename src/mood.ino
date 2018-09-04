#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266mDNS.h>
#include <ESP8266WebServer.h>
#include <FastLed.h>

#define NUM_LEDS       144
#define DATA_PIN       D5
#define LISTEN_PORT    80

CRGB leds[NUM_LEDS];

ESP8266WebServer server(LISTEN_PORT);

const int led = 2;

void handleRoot();
void handleNotFound();
void handleLED();

const char *ssid = "";
const char *password = "";

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(115200);
  wifiConfig();
  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
}

void loop()
{
  // put your main code here, to run repeatedly:
  server.handleClient();
}

void wifiConfig()
{
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin("mood"))
  {
    Serial.println("mDNS responder started.");
  }
  else
  {
    Serial.println("Error setting up mDNS responder!");
  }

  server.on("/", HTTP_GET, handleRoot);
  server.on("/LED", HTTP_GET, handleLed);
  server.on("/OFF", HTTP_GET, handleOff);
  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started.");

  FastLED.setBrightness(150);
}

void handleRoot()
{
  server.send(200, "text/html", "<ul><li><a href='\\LED'>LED Pattern</a></li><li><a href='\\OFF'>LED Off</a></li></ul>");
}

void handleOff()
{
  fill_solid(leds, NUM_LEDS, CRGB::Black);
  FastLED.show();

  server.sendHeader("Location", "/");
  server.send(303);
}

void handleLed()
{
  mtwoRainbow();
  //temp();

  server.sendHeader("Location", "/");
  server.send(303);
}

void handleNotFound()
{
  server.send(404, "text/plain", "404: Not Found");
}

void mtwoRainbow()
{
  int hueDelta = 1;
  int numLEDs = 144;
  int buffer = 36;
  int hueOffset = 128 + buffer;

  // fill main rainbow gradient
  fill_rainbow(leds, numLEDs - buffer - 36, hueOffset, hueDelta);

  // remove some red and add more yellow
  fill_rainbow(&leds[numLEDs - buffer - 36], 36, 270, 1);

  // fill in buffer for smooth transition
  int delta = (numLEDs - buffer) / buffer;
  int index = 0;
  for (int x = 1; x <= buffer; x++)
  {
    leds[numLEDs - x] = leds[index];
    index = index + (delta);
  }
  FastLED.show();

  rotateStrip(10);
}

void rotateStrip(int numRotations)
{
  for (int j = 0; j < numRotations; j++)
  {
    for (int i = 0; i < NUM_LEDS - 1; i++)
    {
      leds[0] = leds[NUM_LEDS - 1];
      memmove8(&leds[1], &leds[0], (NUM_LEDS - 1) * sizeof(CRGB));
      FastLED.show();
      FastLED.delay(40);
    }
  }
}