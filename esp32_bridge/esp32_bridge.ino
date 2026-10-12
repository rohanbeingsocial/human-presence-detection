/*
  ESP32 WiFi bridge for the multi-sensor Arduino Uno

  The Uno still does all the sensing. This board listens to the Uno's
  serial output, re-broadcasts every line over WiFi, and serves the
  dashboard page itself:

    Uno TX (D1) --divider--> ESP32 GPIO16
    browser  --HTTP :80-->   dashboard page (index.html, embedded)
    browser  <--SSE  :81--   every line the Uno prints, as it arrives

  The Uno sketch needs no changes: Serial.print() already drives pin
  D1, so the same text goes to USB and to the ESP32 at once.

  The dashboard is served from here rather than GitHub Pages because a
  page loaded over HTTPS is not allowed to open a plain-HTTP stream to
  a device on your LAN. Same page, different host.

  PIN CONNECTIONS
  -------------------------------
  Uno D1 (TX) -> 10k -> ESP32 RX2 (GPIO16)
  ESP32 RX2   -> 10k -> 10k -> GND   (two 10k in series = 20k)
      The divider drops the Uno's 5V signal to ~3.3V.
      ESP32 pins are NOT 5V tolerant - do not skip it.
  Uno GND     -> ESP32 GND   (serial needs a shared ground)
  ESP32 TX2   -> leave unconnected

  Data only flows Uno -> ESP32, which is what keeps the Uno's D0/D1
  usable. Tapping D1 just listens to what the Uno already sends to
  USB. Wiring TX2 to Uno D0 would fight the Uno's USB chip on that
  pin and break uploads and the Serial Monitor.

  Power the ESP32 from its own USB cable. The Uno's 5V pin can't
  supply the MQ heater and the ESP32's WiFi bursts together.

  BOARD
  -------------------------------
  Tools -> Board -> esp32 -> "ESP32 Dev Module"
  No extra libraries needed.
*/

#include <WiFi.h>
#include <WebServer.h>
#include <ESPmDNS.h>

#include "dashboard_html.h"

// =====================================================
// SETTINGS - edit these
// =====================================================

// The ESP32's own hotspot. Always on, so it works anywhere: join it
// from a phone or laptop and open http://192.168.4.1
const char* AP_SSID = "SensorDashboard";
const char* AP_PASSWORD = "sensors123";  // 8+ characters

// Optional home WiFi, joined as well if it's in range at power-up.
// It lets a PC with no WiFi card reach the dashboard through the
// router. Not found (e.g. at college) means hotspot only. Leave empty
// to skip. ESP32 only sees 2.4 GHz networks.
const char* WIFI_SSID = "GoldCat";
const char* WIFI_PASSWORD = "1234qwer.";

// Also reachable as http://sensors.local
const char* HOSTNAME = "sensors";

#define WIFI_TIMEOUT_MS 10000

// =====================================================
// UNO LINK
// =====================================================

#define UNO_RX_PIN 16
#define UNO_TX_PIN 17     // driven but not wired - only Uno -> ESP32 is used
#define UNO_BAUD 9600     // must match Serial.begin() in the Uno sketch

// Longest line the Uno prints is ~35 chars. Longer input is truncated.
#define MAX_LINE 120

char line[MAX_LINE + 1];
size_t lineLen = 0;

// =====================================================
// SERVERS
// =====================================================

#define STREAM_PORT 81
#define MAX_STREAM_CLIENTS 8   // one per open dashboard tab

WebServer web(80);
NetworkServer streamServer(STREAM_PORT);
NetworkClient streamClients[MAX_STREAM_CLIENTS];

// =====================================================
// SETUP
// =====================================================

void setup() {

  // USB serial is only for debugging this board.
  Serial.begin(115200);

  delay(500);

  Serial.println();
  Serial.println(F("================================"));
  Serial.println(F("  ESP32 SENSOR BRIDGE STARTING"));
  Serial.println(F("================================"));

  // Bigger buffer so lines aren't dropped while a slow browser write
  // briefly holds up loop(). Must be set before begin().
  Serial1.setRxBufferSize(2048);
  Serial1.begin(UNO_BAUD, SERIAL_8N1, UNO_RX_PIN, UNO_TX_PIN);

  startWifi();

  if (MDNS.begin(HOSTNAME)) {
    MDNS.addService("http", "tcp", 80);
  }

  web.on("/", handleDashboard);
  web.onNotFound(handleNotFound);
  web.begin();

  streamServer.begin();

  Serial.println();
  Serial.println(F("Lines received from the Uno are echoed below."));
  Serial.println(F("Nothing appearing? Check the D1 -> GPIO16 wire and GND."));
  Serial.println();
}

// =====================================================
// MAIN LOOP
// =====================================================

void loop() {

  web.handleClient();

  acceptStreamClient();

  readUno();
}

// =====================================================
// WIFI
// =====================================================

void startWifi() {

  WiFi.mode(WIFI_AP_STA);
  WiFi.setHostname(HOSTNAME);

  // Channel 1, visible, up to 8 devices (the ESP32 default is 4).
  WiFi.softAP(AP_SSID, AP_PASSWORD, 1, 0, 8);

  Serial.print(F("Hotspot \""));
  Serial.print(AP_SSID);
  Serial.print(F("\" (password "));
  Serial.print(AP_PASSWORD);
  Serial.println(F(") is up."));
  Serial.print(F("  Join it, then open http://"));
  Serial.println(WiFi.softAPIP());

  if (strlen(WIFI_SSID) == 0) {
    WiFi.mode(WIFI_AP);
    return;
  }

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  Serial.print(F("Looking for home WiFi \""));
  Serial.print(WIFI_SSID);
  Serial.print(F("\" "));

  unsigned long started = millis();

  while (WiFi.status() != WL_CONNECTED && millis() - started < WIFI_TIMEOUT_MS) {
    delay(250);
    Serial.print(F("."));
  }

  Serial.println();

  if (WiFi.status() == WL_CONNECTED) {

    // The ESP32 reconnects on its own if this network drops later.
    Serial.println(F("Also joined home WiFi. From the router side open:"));
    Serial.print(F("  http://"));
    Serial.print(HOSTNAME);
    Serial.println(F(".local"));
    Serial.print(F("  http://"));
    Serial.println(WiFi.localIP());
    return;
  }

  // Stop looking: the ESP32's background retries hop channels, which
  // would keep knocking hotspot users off.
  Serial.println(F("Home WiFi not found. Running as hotspot only."));
  WiFi.disconnect();
  WiFi.mode(WIFI_AP);
}

// =====================================================
// HTTP - serves the dashboard page
// =====================================================

void handleDashboard() {

  // no-cache so a re-flashed page shows up without a hard refresh
  web.sendHeader("Cache-Control", "no-cache");
  web.send_P(200, "text/html; charset=utf-8", DASHBOARD_HTML);
}

void handleNotFound() {
  web.send(404, "text/plain", "Not found");
}

// =====================================================
// STREAM - Server-Sent Events on port 81
//
// Every connection to this port is treated as a subscriber, so there
// is no request to parse: reply with the event-stream header and keep
// the socket. The browser's EventSource reconnects on its own if the
// ESP32 reboots or the WiFi blips.
// =====================================================

void acceptStreamClient() {

  NetworkClient incoming = streamServer.accept();

  if (!incoming) {
    return;
  }

  for (NetworkClient& client : streamClients) {

    if (client.connected()) {
      continue;
    }

    client = incoming;

    // CORS header lets a page from another origin subscribe too,
    // e.g. index.html opened from localhost with ?esp=<ip>.
    client.print(F("HTTP/1.1 200 OK\r\n"
                   "Content-Type: text/event-stream\r\n"
                   "Cache-Control: no-cache\r\n"
                   "Connection: keep-alive\r\n"
                   "Access-Control-Allow-Origin: *\r\n"
                   "\r\n"));
    return;
  }

  // All slots busy. A non-200 makes EventSource give up instead of
  // retrying every few seconds.
  incoming.print(F("HTTP/1.1 503 Service Unavailable\r\n"
                   "Connection: close\r\n"
                   "\r\n"));
  incoming.stop();
}

void broadcast(const char* text) {

  // One write per client keeps each event in a single TCP packet.
  char event[MAX_LINE + 16];
  int eventLen = snprintf(event, sizeof(event), "data: %s\n\n", text);

  for (NetworkClient& client : streamClients) {

    if (!client.connected()) {
      continue;
    }

    // Discard the browser's request headers, which are never parsed.
    while (client.available()) {
      client.read();
    }

    client.write((const uint8_t*)event, eventLen);
  }
}

// =====================================================
// UNO SERIAL - split into lines and broadcast each one
// =====================================================

void readUno() {

  while (Serial1.available()) {

    char c = Serial1.read();

    if (c == '\n') {

      line[lineLen] = '\0';

      // The Uno prints blank spacer lines; nothing to send for those.
      if (lineLen > 0) {
        Serial.println(line);
        broadcast(line);
      }

      lineLen = 0;
      continue;
    }

    // Drops '\r' along with any line noise, so a stray byte can never
    // break the SSE framing or reach the dashboard's parser.
    if (c < 32 || c > 126) {
      continue;
    }

    if (lineLen < MAX_LINE) {
      line[lineLen++] = c;
    }
  }
}
