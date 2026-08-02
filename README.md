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
