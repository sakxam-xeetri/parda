/**
 * School Inauguration Motorized Curtain Control System
 * 
 * Target Board: ESP32 Dev Module
 * Motor Driver: L298N
 * Motors: 2x 12V Johnson Gear Motors
 * Institution: Bhu Pu Sainik Rising Secondary English School
 * Location: Rampur-5, Palpa, Nepal
 * 
 * Features:
 * - Access Point WiFi Hotspot (SSID: INAUGURATION-BHU-PU)
 * - Captive Portal DNS Server (Auto-redirects connected clients to 192.168.4.1)
 * - Non-blocking motor state machine utilizing millis() for auto-stop safety
 * - Adjustable timing configurations saved persistently in Preferences (EEPROM alternative)
 * - Web Dashboard UI loaded from flash memory (PROGMEM)
 */

#include <WiFi.h>
#include <WebServer.h>
#include <DNSServer.h>
#include <Preferences.h>
#include "html.h"

// --- GPIO Pin Configurations ---
#define IN1_PIN 26
#define IN2_PIN 27
#define IN3_PIN 14
#define IN4_PIN 12
#define ENA_PIN 25  // Enable pin for Motor Driver A
#define ENB_PIN 33  // Enable pin for Motor Driver B

// --- System Constants & Objects ---
const char* AP_SSID = "INAUGURATION-BHU-PU";
const char* AP_PASSWORD = "";
const byte DNS_PORT = 53;

IPAddress AP_IP(192, 168, 4, 1);
IPAddress AP_NETMASK(255, 255, 255, 0);

DNSServer dnsServer;
WebServer server(80);
Preferences preferences;

// --- State Machine Declarations ---
enum SystemState {
  STATE_IDLE,
  STATE_OPENING,
  STATE_CLOSING,
  STATE_STOPPED
};

SystemState currentState = STATE_IDLE;
unsigned long motorStartMillis = 0;
unsigned long motorDuration = 8000; // Default 8 seconds run time (in milliseconds)
unsigned long stoppedRecoveryStart = 0;
const unsigned long STOPPED_RECOVERY_TIME = 1500; // Reset from STOPPED state to IDLE after 1.5s

// --- Function Prototypes ---
void setupPins();
void setupWiFi();
void setupWebServer();
void handleRoot();
void handleOpen();
void handleClose();
void handleStop();
void handleStatus();
void handleSetDuration();
void handleNotFound();
void setMotorsStop();
void setMotorsOpen();
void setMotorsClose();
String getSystemStateString();
void sendJSONStatusResponse(const String& message);

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("\n==============================================");
  Serial.println("Bhu Pu Sainik School Smart Curtain Control Booting...");
  Serial.println("==============================================");

  // Initialize GPIO outputs safely
  setupPins();

  // Load persistent configurations
  preferences.begin("curtain_sys", false);
  motorDuration = preferences.getUInt("duration", 8000);
  preferences.end();
  
  Serial.print("Loaded Motor Run Duration: ");
  Serial.print(motorDuration / 1000.0, 1);
  Serial.println(" seconds");

  // Configure Hotspot and DNS
  setupWiFi();

  // Configure HTTP Routes
  setupWebServer();
  
  Serial.println("System fully ready and listening for client commands.");
}

void loop() {
  // Feed DNS Server requests
  dnsServer.processNextRequest();
  
  // Feed HTTP Web Server clients
  server.handleClient();

  // Non-blocking timer checking for Motor Run duration
  unsigned long currentMillis = millis();

  if (currentState == STATE_OPENING || currentState == STATE_CLOSING) {
    if (currentMillis - motorStartMillis >= motorDuration) {
      Serial.println("[AUTO-STOP] Motor run duration elapsed. Stopping motors safely.");
      setMotorsStop();
      currentState = STATE_IDLE;
    }
  } 
  else if (currentState == STATE_STOPPED) {
    // Gracefully recover to IDLE after a short pause so buttons become operational again
    if (currentMillis - stoppedRecoveryStart >= STOPPED_RECOVERY_TIME) {
      currentState = STATE_IDLE;
      Serial.println("[STATE] Recovered from STOPPED to IDLE state.");
    }
  }
}

// --- Hardware Control Implementations ---

void setupPins() {
  pinMode(IN1_PIN, OUTPUT);
  pinMode(IN2_PIN, OUTPUT);
  pinMode(IN3_PIN, OUTPUT);
  pinMode(IN4_PIN, OUTPUT);
  pinMode(ENA_PIN, OUTPUT);
  pinMode(ENB_PIN, OUTPUT);

  // Drive all outputs LOW at start to ensure motor safety
  setMotorsStop();
  Serial.println("GPIO Pin mapping initialized and driven LOW (safe mode).");
}

void setMotorsStop() {
  digitalWrite(IN1_PIN, LOW);
  digitalWrite(IN2_PIN, LOW);
  digitalWrite(IN3_PIN, LOW);
  digitalWrite(IN4_PIN, LOW);
  digitalWrite(ENA_PIN, LOW);
  digitalWrite(ENB_PIN, LOW);
}

void setMotorsOpen() {
  // Motor A and Motor B Forward (both open curtain)
  digitalWrite(IN1_PIN, HIGH);
  digitalWrite(IN2_PIN, LOW);
  digitalWrite(IN3_PIN, HIGH);
  digitalWrite(IN4_PIN, LOW);
  
  // Enable motor driver output
  digitalWrite(ENA_PIN, HIGH);
  digitalWrite(ENB_PIN, HIGH);
}

void setMotorsClose() {
  // Motor A and Motor B Reverse (both close curtain)
  digitalWrite(IN1_PIN, LOW);
  digitalWrite(IN2_PIN, HIGH);
  digitalWrite(IN3_PIN, LOW);
  digitalWrite(IN4_PIN, HIGH);
  
  // Enable motor driver output
  digitalWrite(ENA_PIN, HIGH);
  digitalWrite(ENB_PIN, HIGH);
}

// --- WiFi AP Setup ---

void setupWiFi() {
  Serial.print("Configuring Access Point: ");
  Serial.println(AP_SSID);

  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(AP_IP, AP_IP, AP_NETMASK);
  
  // Start soft AP (SSID, Password, Channel, Hide, MaxConnections)
  bool success = WiFi.softAP(AP_SSID, AP_PASSWORD, 1, 0, 4);
  
  if (success) {
    Serial.print("AP successfully created. Gateway IP: ");
    Serial.println(WiFi.softAPIP());
  } else {
    Serial.println("SoftAP creation failed!");
  }

  // Setup local DNS redirection to redirect any domain requests to 192.168.4.1
  dnsServer.setErrorReplyCode(DNSReplyCode::NoError);
  dnsServer.start(DNS_PORT, "*", AP_IP);
  Serial.println("DNS Redirect Server started on port 53.");
}

// --- Web Server Routing ---

void setupWebServer() {
  server.on("/", HTTP_GET, handleRoot);
  server.on("/open", HTTP_POST, handleOpen);
  server.on("/open", HTTP_GET, handleOpen); // Support GET for easier manual browser tests
  
  server.on("/close", HTTP_POST, handleClose);
  server.on("/close", HTTP_GET, handleClose);
  
  server.on("/stop", HTTP_POST, handleStop);
  server.on("/stop", HTTP_GET, handleStop);
  
  server.on("/status", HTTP_GET, handleStatus);
  
  server.on("/set_duration", HTTP_POST, handleSetDuration);
  server.on("/set_duration", HTTP_GET, handleSetDuration);
  
  server.onNotFound(handleNotFound);

  // Set HTTP server CORS headers (optional, helpful during dev testing)
  server.enableCORS(true);
  
  server.begin();
  Serial.println("HTTP Web Server started on port 80.");
}

// Serves the Web Dashboard UI
void handleRoot() {
  server.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
  server.sendHeader("Pragma", "no-cache");
  server.sendHeader("Expires", "-1");
  server.send(200, "text/html", INDEX_HTML);
}

// Triggers Opening Motion
void handleOpen() {
  if (currentState == STATE_OPENING) {
    sendJSONStatusResponse("Already opening.");
    return;
  }
  
  Serial.println("[ACTION] OPEN CURTAIN triggered.");
  setMotorsOpen();
  currentState = STATE_OPENING;
  motorStartMillis = millis();
  
  sendJSONStatusResponse("Curtain opening initiated.");
}

// Triggers Closing Motion
void handleClose() {
  if (currentState == STATE_CLOSING) {
    sendJSONStatusResponse("Already closing.");
    return;
  }
  
  Serial.println("[ACTION] CLOSE CURTAIN triggered.");
  setMotorsClose();
  currentState = STATE_CLOSING;
  motorStartMillis = millis();
  
  sendJSONStatusResponse("Curtain closing initiated.");
}

// Emergency Stop
void handleStop() {
  Serial.println("[ACTION] EMERGENCY STOP triggered.");
  setMotorsStop();
  currentState = STATE_STOPPED;
  stoppedRecoveryStart = millis();
  
  sendJSONStatusResponse("Emergency stop activated.");
}

// Returns current status, duration configs, and timing metadata as JSON
void handleStatus() {
  sendJSONStatusResponse("Status read.");
}

// Configure Motor Run Duration
void handleSetDuration() {
  if (server.hasArg("seconds")) {
    float secondsVal = server.arg("seconds").toFloat();
    if (secondsVal >= 3.0 && secondsVal <= 20.0) {
      motorDuration = (unsigned long)(secondsVal * 1000.0);
      
      // Save permanently to flash
      preferences.begin("curtain_sys", false);
      preferences.putUInt("duration", motorDuration);
      preferences.end();
      
      Serial.print("[CONFIG] Updated motor run duration to: ");
      Serial.print(secondsVal, 1);
      Serial.println("s");
      
      sendJSONStatusResponse("Duration updated successfully.");
      return;
    }
  }
  
  server.send(400, "application/json", "{\"status\":\"error\",\"message\":\"Invalid seconds parameter. Must be between 3.0 and 20.0\"}");
}

// Captive Portal Redirect Handler
void handleNotFound() {
  String host = server.hostHeader();
  // If the host requested is not our AP IP, redirect them to the captive portal home page
  if (host != "192.168.4.1" && host != "localhost") {
    Serial.print("[DNS CAPTIVE] Redirecting host ");
    Serial.print(host);
    Serial.println(" to http://192.168.4.1/");
    
    server.sendHeader("Location", "http://192.168.4.1/", true);
    server.send(302, "text/plain", ""); // Redirect code
    return;
  }
  
  server.send(404, "text/plain", "Endpoint Not Found");
}

// --- Helper Functions ---

String getSystemStateString() {
  switch (currentState) {
    case STATE_IDLE:     return "IDLE";
    case STATE_OPENING:  return "OPENING";
    case STATE_CLOSING:  return "CLOSING";
    case STATE_STOPPED:  return "STOPPED";
    default:             return "UNKNOWN";
  }
}

void sendJSONStatusResponse(const String& message) {
  unsigned long timeRemaining = 0;
  if (currentState == STATE_OPENING || currentState == STATE_CLOSING) {
    unsigned long elapsed = millis() - motorStartMillis;
    if (elapsed < motorDuration) {
      timeRemaining = motorDuration - elapsed;
    }
  }

  String json = "{";
  json += "\"status\":\"success\",";
  json += "\"state\":\"" + getSystemStateString() + "\",";
  json += "\"motorDuration\":" + String(motorDuration) + ",";
  json += "\"timeRemaining\":" + String(timeRemaining) + ",";
  json += "\"message\":\"" + message + "\"";
  json += "}";
  
  server.send(200, "application/json", json);
}
