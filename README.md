# Arduino Sensor Dashboard

A single-file live dashboard for the `main.ino` multi-sensor sketch. Reads the
Arduino's USB serial output directly in the browser and plots it — no server,
no backend, no build step.

## How it works

GitHub Pages only serves static files, so it can't read your USB port. Instead
the page uses the **Web Serial API**, which runs entirely in the browser and
opens the port from the client side. That's why this works on Pages at all.

The page parses the human-readable text `main.ino` already prints, so **the
sketch needs no changes**. It keys off these lines:

```
Distance: 12.3 cm          ->  distance   (also handles "OUT OF RANGE")
Gas Analog: 456            ->  gas level
Gas Digital: 1             ->  gas alarm  (active-LOW: 0 means gas detected)
Object Temp: 36.5 C        ->  IR object temp   (also handles "N/A")
Ambient Temp: 24.0 C       ->  IR ambient temp  (also handles "N/A")
UID: 04 A3 B2 C1           ->  RFID scan event
```

A sample is committed when `Ambient Temp` arrives, since that's the last field
the sketch emits per loop.

## Browser support — read this first

| Browser | Works? |
|---|---|
| Chrome / Edge / Opera, **desktop** | Yes |
| Firefox, Safari | No — Web Serial not implemented |
| Anything on phone/tablet | No |

This is a browser limitation, not a bug in the page. On an unsupported browser
the dashboard still loads and the **Demo data** button still works — you just
can't open a real port.

## Deploying to GitHub Pages

1. Create a repo on GitHub (public, or private with Pages enabled).
2. Put `index.html` at the **repo root**.
3. Commit and push:
   ```bash
   git init
   git add index.html README.md
   git commit -m "Arduino sensor dashboard"
   git branch -M main
   git remote add origin https://github.com/<you>/<repo>.git
   git push -u origin main
   ```
4. On GitHub: **Settings → Pages → Source = Deploy from a branch**, branch
   `main`, folder `/ (root)`. Save.
5. Wait ~1 minute, then open `https://<you>.github.io/<repo>/`.

Pages serves over HTTPS, which is what Web Serial requires. Nothing else to configure.

## Using it

1. Upload `main.ino` to the Arduino.
2. **Close the Arduino IDE's Serial Monitor.** Only one program can hold a COM
   port at a time — if the Serial Monitor is open, the browser can't connect.
   This is the single most common reason "Connect" appears to do nothing.
3. Open the Pages URL in Chrome or Edge.
4. Click **Connect Arduino** and pick your board's port from the dialog.
5. Readings start appearing within about a second.

Note that opening the port resets the Arduino (DTR), so you'll briefly see the
sketch's startup banner in the raw serial panel before readings begin.

## Testing locally

Web Serial needs a secure context, so open the page over `http://localhost`
rather than double-clicking the file — a `file://` URL is not reliable here.

```bash
cd dashboard
python -m http.server 8000
# then open http://localhost:8000
```

Or press **Demo data** on any browser to drive the dashboard with synthetic
readings and no hardware attached — useful for checking the layout before you
have the wiring finished.

## WiFi mode (ESP32 bridge)

Adds an ESP32 so the laptop doesn't need a USB cable to the Uno. The Uno
sketch is unchanged: its `Serial.print` output already appears on pin D1,
so the ESP32 just listens there and re-broadcasts each line over WiFi.

```
Uno --D1 (serial)--> ESP32 --WiFi--> browser on any device
```

The ESP32 also **serves the dashboard itself**. That's not optional: a page
loaded over HTTPS (GitHub Pages) is blocked from opening a plain-HTTP
connection to a device on your LAN, so the Pages copy can't reach the ESP32.
Served from the ESP32 it's the same page, and it works in any browser,
phones included, since Web Serial isn't involved.

### Wiring

Needs three 10 kΩ resistors.

```
Uno D1 (TX) ── 10k ──┬── ESP32 RX2 (GPIO16)
                     │
                    10k
                     │
                    10k
                     │
                    GND
```

| From | To |
|---|---|
| Uno D1 (TX) | through the divider above → ESP32 RX2 |
| Uno GND | ESP32 GND |
| ESP32 TX2 | nothing — leave it unconnected |
| Uno D0 (RX) | nothing — leave it unconnected |

The divider drops the Uno's 5 V signal to 5 × 20k / 30k ≈ 3.3 V. **ESP32 pins
are not 5 V tolerant**, so don't wire D1 straight across. With only two 10k
(a 1:1 split) you'd get 2.5 V, which is right at the ESP32's threshold and
will drop bytes. Use all three.

**Why this avoids the D0/D1 conflict:** data only goes Uno → ESP32. D1 is the
Uno's TX output, so the ESP32 just listens in on what the Uno already sends to
USB. Uploads and the Serial Monitor keep working. The conflict only happens
if something *drives* D0, so ESP32 TX2 must stay off Uno D0. No
`SoftwareSerial` needed.

Power the ESP32 from its own USB cable or phone charger. The Uno's 5 V pin
can't supply the MQ heater plus the ESP32's WiFi bursts, and you'll get
random resets.

### Flashing the ESP32

1. Arduino IDE → Boards Manager → install **esp32 by Espressif** (3.x).
2. Open `esp32_bridge/esp32_bridge.ino`. Add a new tab (the `⋯` menu → New
   Tab) named `wifi_secrets.h` containing:
   ```cpp
   #define WIFI_SSID     "your-wifi-name"
   #define WIFI_PASSWORD "your-wifi-password"
   ```
   Git ignores this file, so your password stays off GitHub. Skip it and the
   ESP32 just starts its own network (below).
3. Board: **ESP32 Dev Module**. Upload. No extra libraries needed.
4. Open the ESP32's Serial Monitor at **115200** baud. It prints the address
   to open, then echoes every line it receives from the Uno, which is the
   quickest way to confirm the D1 wire is right.

### Opening the dashboard

Laptop on the same WiFi as the ESP32, then open **http://sensors.local** (or
the IP the Serial Monitor printed). It connects by itself.

If the ESP32 can't join your WiFi within 15 s it **creates its own network**
called `SensorDashboard` (password `sensors123`). Join that from the laptop and
open **http://192.168.4.1**. Use this for demos: college and office WiFi often
block devices from talking to each other, and the ESP32 only sees 2.4 GHz
networks.

### After editing index.html

The ESP32 serves a copy compiled into its flash, so regenerate it and
re-upload:

```bash
python embed_dashboard.py
```

To try dashboard edits without re-flashing, serve the folder locally and point
it at the ESP32: `http://localhost:8000/?esp=192.168.1.50`.

## Features

- Live stat tiles with sparklines for distance, gas, object and ambient temperature
- Gas alarm status derived from the MQ digital pin (icon + label, never colour alone)
- Time-series charts with hover crosshair and tooltip
- Time-window filter (1 / 5 / 15 min / all) scoping every chart at once
- RFID scan log
- Table view with every numeric reading
- Raw serial console for debugging
- CSV export of the whole session
- Light and dark themes, both colour-validated for colour-vision deficiency

## Chart design notes

Object and ambient temperature share one chart because they share a unit (°C)
and therefore one axis. Distance (cm) and gas (ADC counts) get their own charts
rather than being overlaid on a second y-axis — a dual-axis chart invents a
correlation that isn't in the data.

Series colours are fixed per sensor and never reassigned, so a colour always
means the same reading. Out-of-range distance readings break the line into a
gap instead of plotting as zero.
