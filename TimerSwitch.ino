#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <LittleFS.h>
#include <ArduinoJson.h>

// Data
int duration;
int unit;
bool durationOrUnitChanged = true;
uint32_t startTime;
uint64_t timeElapsed;


// Wifi Credentials
const char* ssid = "YOUR_SSID";
const char* password = "YOUR_PASSWORD";

ESP8266WebServer server(80);

IPAddress local_IP(192,168,1,50);
IPAddress gateway(192,168,1,1);
IPAddress subnet(255, 255, 0, 0);
IPAddress primaryDNS(8, 8, 8, 8); 


void setup() {
  // Serial init
  Serial.begin(9600);

  // FS Init
  LittleFS.begin();

  // Static IP Config
  if(!WiFi.config(local_IP, gateway, subnet, primaryDNS)) {
    Serial.println("Failed to configure static IP");
  }

  // Connecting to WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
  }
  Serial.println("WiFi Connected");
  Serial.println(WiFi.localIP());

  // Initializing WebServer
  server.serveStatic("/", LittleFS, "/index.html");
  server.serveStatic("/style.css", LittleFS, "/style.css");
  server.serveStatic("/script.js", LittleFS, "/script.js");

  // Creating Endpoints


  // NORMAL CONTROL
  server.on("/control", HTTP_POST, []() {  //[]() are cpp anonymous function declarations
    String body = server.arg("plain");
    JsonDocument doc;

    // ERROR HANDLING
    DeserializationError err = deserializeJson(doc,body);
    if (err) {
      server.send(400, "text/plain", "Invalid JSON");
      Serial.println("Transmission Failed");
      return;
    } else {
      server.send(200, "text/plain", "OK");
      Serial.println("Input Received from webpage");
    }

    // Parsing JSON
    duration = doc["duration"];
    unit = doc["unit"];

    // Setting Flag
    durationOrUnitChanged = true;
    Serial.println("Timer Modified");
  });


  // STATUS REPORTING
  server.on("/status", HTTP_GET, []() {
    JsonDocument doc;
    doc["rxdDuration"] = duration;
    doc["rxdUnit"] = (unit==60000)?"minutes":"hours";
    String rxdData;
    serializeJson(doc, rxdData);
    server.send(200, "application/json", rxdData);
  });


  // EMERGENCY STOP
  server.on("/reset", HTTP_POST, []() {
    server.send(200, "text/plain", "Restarting");
    ESP.restart();
  });

  server.begin();

  pinMode(14, OUTPUT);
  digitalWrite(14, LOW);
}

void loop() {
  // put your main code here, to run repeatedly:
  server.handleClient();
  

  // handling changed inputs mid-execution
  if (durationOrUnitChanged) {
    timeElapsed = 0;
    startTime = millis();
    digitalWrite(14, HIGH);
    durationOrUnitChanged = false;

    // Sending back set values for confirmation
    
  }

  // Delay Logic
  if (timeElapsed >= (duration*unit)){  //unit is the multiplier (60000 for minute, 3600000 for hour) to get time in milliseconds
    digitalWrite(14, LOW);
  }
  else {
    timeElapsed = millis() - startTime;
  }





}
