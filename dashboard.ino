/*
  Multi-Sensor Arduino Uno - serial feed for the web dashboard

  No LCD. Readings go out over USB serial and are plotted by
  index.html in this same folder (Web Serial API, Chrome/Edge).

  The output format below is what the dashboard parses. If you change
  the wording of these Serial.print lines, update the parser in
  index.html to match.

  PIN CONNECTIONS
  -------------------------------
  HC-SR04:
    TRIG -> D7
    ECHO -> D8
    VCC  -> 5V
    GND  -> GND

  MQ Sensor:
    AO   -> A0
    DO   -> D2
    VCC  -> 5V
    GND  -> GND

  MLX90614:
    SDA  -> A4
    SCL  -> A5
    VCC  -> 5V (GY-906 breakout with onboard regulator)
    GND  -> GND

  RC522:
    SDA/SS -> D10
    SCK    -> D13
    MOSI   -> D11
    MISO   -> D12
    RST    -> D9
    VCC    -> 3.3V  (NOT 5V - this will destroy the module)
    GND    -> GND
*/

#include <SPI.h>
#include <MFRC522.h>
#include <Wire.h>
#include <Adafruit_MLX90614.h>

// =====================================================
// HC-SR04
// =====================================================

#define TRIG_PIN 7
#define ECHO_PIN 8

// =====================================================
// MQ GAS SENSOR
// =====================================================

#define MQ_AO_PIN A0
#define MQ_DO_PIN 2

// =====================================================
// RC522 RFID
// =====================================================

#define RST_PIN 9
#define SS_PIN 10

MFRC522 rfid(SS_PIN, RST_PIN);
bool rfidReady = false;

// =====================================================
// MLX90614
// =====================================================

Adafruit_MLX90614 mlx = Adafruit_MLX90614();
bool mlxReady = false;

// =====================================================
// SETUP
// =====================================================

void setup() {

  // ---------------------------------------------------
  // SERIAL
  // ---------------------------------------------------

  Serial.begin(9600);

  delay(500);

  Serial.println();
  Serial.println(F("================================"));
  Serial.println(F("  MULTI SENSOR SYSTEM STARTING"));
  Serial.println(F("================================"));
  Serial.println();

  // ---------------------------------------------------
  // PIN MODES
  // ---------------------------------------------------

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  pinMode(MQ_DO_PIN, INPUT);

  // ---------------------------------------------------
  // I2C
  // ---------------------------------------------------

  Wire.begin();

  // Without a timeout the AVR Wire library spins forever inside
  // endTransmission() if the bus is ever jammed, which would kill the
  // serial stream and leave the dashboard showing nothing. 25ms with
  // reset-on-timeout keeps the sketch alive even if the MLX misbehaves.
  // twi_init() does not clear this, so it survives later Wire.begin()
  // calls made inside libraries.
  Wire.setWireTimeout(25000, true);

  // ---------------------------------------------------
  // MLX90614
  // ---------------------------------------------------

  Serial.print(F("Initializing MLX90614... "));

  mlxReady = mlx.begin();

  if (mlxReady) {

    Serial.println(F("OK"));

  } else {

    Serial.println(F("FAILED"));
    Serial.println(F("  Not responding at 0x5A. Check SDA (A4) and SCL (A5)."));
    Serial.println(F("  Temperatures will report N/A."));
  }

  // ---------------------------------------------------
  // RFID
  // ---------------------------------------------------

  Serial.print(F("Initializing RC522... "));

  SPI.begin();

  rfid.PCD_Init();

  delay(100);

  // A healthy MFRC522 reports 0x91 or 0x92 here. 0x00 and 0xFF
  // both mean the SPI link is dead rather than the chip is busy.
  byte version = rfid.PCD_ReadRegister(MFRC522::VersionReg);

  rfidReady = (version != 0x00 && version != 0xFF);

  if (rfidReady) {

    Serial.print(F("OK (version 0x"));
    Serial.print(version, HEX);
    Serial.println(F(")"));

  } else {

    Serial.println(F("FAILED"));
    Serial.println(F("  No SPI response. Check D9-D13 and 3.3V power."));
  }

  // ---------------------------------------------------

  Serial.println();
  Serial.println(F("================================"));
  Serial.println(F("   SYSTEM INITIALIZED"));
  Serial.println(F("================================"));
  Serial.println();

  delay(500);
}

// =====================================================
// MAIN LOOP
// =====================================================

void loop() {

  // ---------------------------------------------------
  // READ SENSORS
  // ---------------------------------------------------

  float distanceCM = readUltrasonic();

  int gasValue = analogRead(MQ_AO_PIN);
  int gasDigital = digitalRead(MQ_DO_PIN);

  // A disconnected MLX90614 returns NaN rather than failing loudly,
  // which is why every use below is guarded.
  float objectTemp = mlxReady ? mlx.readObjectTempC() : NAN;
  float ambientTemp = mlxReady ? mlx.readAmbientTempC() : NAN;

  // ---------------------------------------------------
  // SERIAL OUTPUT  ->  parsed by the dashboard
  //
  // The dashboard commits one sample when "Ambient Temp" arrives,
  // so that line must stay last in this block.
  // ---------------------------------------------------

  Serial.println(F("--------------------------------"));

  Serial.print(F("Distance: "));

  if (distanceCM < 0) {

    Serial.println(F("OUT OF RANGE"));

  } else {

    Serial.print(distanceCM, 1);
    Serial.println(F(" cm"));
  }

  Serial.print(F("Gas Analog: "));
  Serial.println(gasValue);

  Serial.print(F("Gas Digital: "));
  Serial.println(gasDigital);

  Serial.print(F("Object Temp: "));

  if (isnan(objectTemp)) {

    Serial.println(F("N/A"));

  } else {

    Serial.print(objectTemp, 1);
    Serial.println(F(" C"));
  }

  Serial.print(F("Ambient Temp: "));

  if (isnan(ambientTemp)) {

    Serial.println(F("N/A"));

  } else {

    Serial.print(ambientTemp, 1);
    Serial.println(F(" C"));
  }

  // ---------------------------------------------------
  // CHECK RFID
  // ---------------------------------------------------

  checkRFID();

  // ---------------------------------------------------

  delay(1000);
}

// =====================================================
// HC-SR04 FUNCTION
// =====================================================

float readUltrasonic() {

  // Ensure trigger starts LOW

  digitalWrite(TRIG_PIN, LOW);

  delayMicroseconds(2);

  // Send 10 microsecond pulse

  digitalWrite(TRIG_PIN, HIGH);

  delayMicroseconds(10);

  digitalWrite(TRIG_PIN, LOW);

  // Measure echo pulse, 30 ms timeout (~5 m)

  unsigned long duration = pulseIn(ECHO_PIN, HIGH, 30000);

  // No echo received

  if (duration == 0) {
    return -1;
  }

  // Convert time to distance

  return duration * 0.0343 / 2.0;
}

// =====================================================
// RC522 RFID FUNCTION
// =====================================================

void checkRFID() {

  if (!rfidReady) {
    return;
  }

  if (!rfid.PICC_IsNewCardPresent()) {
    return;
  }

  if (!rfid.PICC_ReadCardSerial()) {
    return;
  }

  // ---------------------------------------------------
  // SERIAL OUTPUT
  //
  // "UID: 04 A3 B2 C1" - space-separated hex, which is the form the
  // dashboard's UID regex expects.
  // ---------------------------------------------------

  Serial.println();
  Serial.println(F("*** RFID CARD DETECTED ***"));

  Serial.print(F("UID:"));

  for (byte i = 0; i < rfid.uid.size; i++) {

    Serial.print(F(" "));

    if (rfid.uid.uidByte[i] < 0x10) {
      Serial.print(F("0"));
    }

    Serial.print(rfid.uid.uidByte[i], HEX);
  }

  Serial.println();

  // ---------------------------------------------------

  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();

  // The original sketch paused 2s here so the UID stayed readable on
  // the LCD. With no display to hold, that pause would only stall the
  // dashboard's data stream, so it is gone.
}
