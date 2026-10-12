// Generated from index.html by embed_dashboard.py - do not edit by hand.
#pragma once

const char DASHBOARD_HTML[] PROGMEM = R"HTML(<!doctype html>
<html lang="en">
<head>
<meta charset="utf-8">
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>Arduino Sensor Dashboard</title>
<style>
/* ============================================================
   TOKENS
   ============================================================ */
:root {
  color-scheme: light;
  --plane:          #f9f9f7;
  --surface-1:      #fcfcfb;
  --text-primary:   #0b0b0b;
  --text-secondary: #52514e;
  --text-muted:     #898781;
  --gridline:       #e1e0d9;
  --axis:           #c3c2b7;
  --border:         rgba(11,11,11,0.10);

  /* Series colours follow the ENTITY, never its position on screen. */
  --s-ambient:  #2a78d6;  /* slot 1 blue   */
  --s-object:   #eb6834;  /* slot 2 orange */
  --s-distance: #1baf7a;  /* slot 3 aqua   */
  --s-gas:      #4a3aa7;  /* slot 7 violet */

  /* Status tokens are reserved - never reused as a series colour. */
  --st-good:     #0ca30c;
  --st-critical: #d03b3b;
  --st-muted:    #898781;
}
@media (prefers-color-scheme: dark) {
  :root:where(:not([data-theme="light"])) {
    color-scheme: dark;
    --plane:          #0d0d0d;
    --surface-1:      #1a1a19;
    --text-primary:   #ffffff;
    --text-secondary: #c3c2b7;
    --text-muted:     #898781;
    --gridline:       #2c2c2a;
    --axis:           #383835;
    --border:         rgba(255,255,255,0.10);
    --s-ambient:  #3987e5;
    --s-object:   #d95926;
    --s-distance: #199e70;
    --s-gas:      #9085e9;
  }
}
:root[data-theme="dark"] {
  color-scheme: dark;
  --plane:          #0d0d0d;
  --surface-1:      #1a1a19;
  --text-primary:   #ffffff;
  --text-secondary: #c3c2b7;
  --text-muted:     #898781;
  --gridline:       #2c2c2a;
  --axis:           #383835;
  --border:         rgba(255,255,255,0.10);
  --s-ambient:  #3987e5;
  --s-object:   #d95926;
  --s-distance: #199e70;
  --s-gas:      #9085e9;
}

* { box-sizing: border-box; }
html, body { overflow-x: hidden; }
body {
  margin: 0;
  background: var(--plane);
  color: var(--text-primary);
  font: 15px/1.5 system-ui, -apple-system, "Segoe UI", sans-serif;
  -webkit-font-smoothing: antialiased;
}
.wrap { max-width: 1180px; margin: 0 auto; padding: 24px 20px 64px; }

/* ============================================================
   HEADER
   ============================================================ */
header {
  display: flex; flex-wrap: wrap; gap: 16px;
  align-items: center; justify-content: space-between;
  margin-bottom: 8px;
}
h1 { font-size: 21px; font-weight: 650; margin: 0; letter-spacing: -0.01em; }
.sub { color: var(--text-secondary); font-size: 13.5px; margin: 2px 0 22px; }
.controls { display: flex; flex-wrap: wrap; gap: 8px; align-items: center; }

button, select {
  font: inherit; font-size: 13.5px;
  color: var(--text-primary);
  background: var(--surface-1);
  border: 1px solid var(--border);
  border-radius: 8px;
  padding: 7px 13px;
  cursor: pointer;
}
button:hover, select:hover { border-color: var(--axis); }
button.primary {
  background: var(--s-ambient); color: #fff; border-color: transparent; font-weight: 550;
}
button.primary:hover { filter: brightness(1.08); }
button:disabled { opacity: .45; cursor: not-allowed; }

.chip {
  display: inline-flex; align-items: center; gap: 7px;
  font-size: 13px; font-weight: 550;
  padding: 6px 12px; border-radius: 999px;
  border: 1px solid var(--border); background: var(--surface-1);
  color: var(--text-secondary);
}
.dot { width: 8px; height: 8px; border-radius: 50%; background: var(--st-muted); flex: none; }
.chip.live .dot { background: var(--st-good); }
.chip.err  .dot { background: var(--st-critical); }

/* ============================================================
   BANNERS
   ============================================================ */
.banner {
  border: 1px solid var(--border); border-left: 3px solid var(--st-critical);
  background: var(--surface-1); border-radius: 8px;
  padding: 13px 16px; margin-bottom: 20px; font-size: 13.5px;
  color: var(--text-secondary);
}
.banner strong { color: var(--text-primary); }
.banner.hidden { display: none; }

/* ============================================================
   ALERT ROW  (gas digital = status, so icon + label, never colour alone)
   ============================================================ */
.alert {
  display: flex; align-items: center; gap: 10px;
  padding: 12px 16px; border-radius: 10px;
  border: 1px solid var(--border); background: var(--surface-1);
  margin-bottom: 20px; font-size: 14px; font-weight: 550;
}
.alert svg { flex: none; }
.alert.ok       { border-left: 3px solid var(--st-good); }
.alert.critical { border-left: 3px solid var(--st-critical); }
.alert .note { font-weight: 400; color: var(--text-secondary); margin-left: auto; font-size: 13px; }

/* ============================================================
   FILTER ROW - one row, scopes every chart below it
   ============================================================ */
.filters {
  display: flex; gap: 8px; align-items: center; flex-wrap: wrap;
  margin-bottom: 16px;
}
.filters .label { font-size: 13px; color: var(--text-secondary); margin-right: 2px; }
.seg { display: inline-flex; border: 1px solid var(--border); border-radius: 8px; overflow: hidden; }
.seg button { border: none; border-radius: 0; background: var(--surface-1); padding: 7px 14px; }
.seg button + button { border-left: 1px solid var(--border); }
.seg button[aria-pressed="true"] { background: var(--s-ambient); color: #fff; font-weight: 550; }

/* ============================================================
   STAT TILES
   ============================================================ */
.tiles {
  display: grid; gap: 14px; margin-bottom: 22px;
  grid-template-columns: repeat(auto-fit, minmax(210px, 1fr));
}
.tile {
  background: var(--surface-1); border: 1px solid var(--border);
  border-radius: 12px; padding: 16px 16px 10px;
}
.tile .name {
  display: flex; align-items: center; gap: 7px;
  font-size: 12.5px; font-weight: 550; color: var(--text-secondary);
  text-transform: uppercase; letter-spacing: .045em;
}
.swatch { width: 9px; height: 9px; border-radius: 2px; flex: none; }
/* Proportional figures on hero values - tabular-nums only in table columns. */
.tile .value {
  font-size: 33px; font-weight: 620; letter-spacing: -0.02em;
  margin: 9px 0 0; line-height: 1.05; color: var(--text-primary);
}
.tile .unit { font-size: 15px; font-weight: 500; color: var(--text-muted); margin-left: 3px; }
.tile .spark { height: 34px; margin-top: 6px; }
.tile .spark svg { display: block; width: 100%; height: 34px; }

/* ============================================================
   CHART CARDS
   ============================================================ */
.grid2 { display: grid; gap: 14px; grid-template-columns: 1fr 1fr; }
@media (max-width: 820px) { .grid2 { grid-template-columns: 1fr; } }

.card {
  background: var(--surface-1); border: 1px solid var(--border);
  border-radius: 12px; padding: 16px 18px 14px; margin-bottom: 14px;
}
.card h2 {
  font-size: 14px; font-weight: 600; margin: 0 0 2px;
  color: var(--text-primary); letter-spacing: -0.005em;
}
.card .cap { font-size: 12.5px; color: var(--text-muted); margin: 0 0 12px; }

.legend { display: flex; gap: 16px; flex-wrap: wrap; margin: 0 0 10px; }
.legend .item {
  display: inline-flex; align-items: center; gap: 7px;
  font-size: 13px; color: var(--text-secondary);
}

/* The plot host grows with content so the x-axis band is never clipped. */
.plot { position: relative; width: 100%; }
.plot svg { display: block; width: 100%; }

.tip {
  position: absolute; pointer-events: none; opacity: 0;
  transition: opacity .09s;
  background: var(--surface-1); border: 1px solid var(--border);
  border-radius: 8px; padding: 9px 11px;
  font-size: 12.5px; line-height: 1.5;
  box-shadow: 0 4px 16px rgba(0,0,0,.13);
  white-space: nowrap; z-index: 5;
  color: var(--text-primary);
}
.tip .tt { color: var(--text-muted); font-size: 11.5px; margin-bottom: 3px; }
.tip .row { display: flex; align-items: center; gap: 7px; }
.tip .row b { font-weight: 600; font-variant-numeric: tabular-nums; margin-left: auto; padding-left: 14px; }

/* ============================================================
   TABLES  (the WCAG-clean twin of every chart)
   ============================================================ */
.tablewrap { overflow-x: auto; }
table { border-collapse: collapse; width: 100%; font-size: 13px; }
th, td {
  text-align: right; padding: 7px 12px;
  border-bottom: 1px solid var(--gridline);
  font-variant-numeric: tabular-nums;
  white-space: nowrap;
}
th {
  color: var(--text-secondary); font-weight: 600; font-size: 12px;
  text-transform: uppercase; letter-spacing: .04em;
  position: sticky; top: 0; background: var(--surface-1);
}
th:first-child, td:first-child { text-align: left; }
tbody tr:last-child td { border-bottom: none; }
.scroll { max-height: 320px; overflow-y: auto; }
.empty { color: var(--text-muted); font-size: 13.5px; padding: 14px 0; }

.console {
  font: 12px/1.65 ui-monospace, "Cascadia Code", Consolas, monospace;
  background: var(--plane); border: 1px solid var(--border); border-radius: 8px;
  padding: 11px 13px; max-height: 260px; overflow: auto;
  white-space: pre-wrap; word-break: break-word; color: var(--text-secondary);
}
details { margin-bottom: 14px; }
summary {
  cursor: pointer; font-size: 13.5px; font-weight: 550;
  color: var(--text-secondary); padding: 9px 0; list-style: none;
}
summary::-webkit-details-marker { display: none; }
summary::before { content: "▸ "; color: var(--text-muted); }
details[open] summary::before { content: "▾ "; }
</style>
</head>
<body>
<div class="wrap">

  <header>
    <div>
      <h1>Arduino Sensor Dashboard</h1>
    </div>
    <div class="controls">
      <select id="baud" aria-label="Baud rate">
        <option value="9600" selected>9600 baud</option>
        <option value="19200">19200 baud</option>
        <option value="57600">57600 baud</option>
        <option value="115200">115200 baud</option>
      </select>
      <button id="connect" class="primary">Connect Arduino</button>
      <button id="demo">Demo data</button>
      <button id="csv">Export CSV</button>
      <button id="theme" aria-label="Toggle colour theme">◐</button>
      <span id="status" class="chip"><span class="dot"></span><span id="statusText">Disconnected</span></span>
    </div>
  </header>
  <p class="sub" id="sub">Live readings streamed over USB from <code>main.ino</code> — ultrasonic, gas, IR temperature and RFID.</p>

  <div id="unsupported" class="banner hidden">
    <strong>This browser can't talk to the Arduino.</strong>
    The Web Serial API is only implemented in Chrome, Edge and Opera on desktop.
    Firefox, Safari and all mobile browsers can't open a USB port from a web page.
    You can still press <strong>Demo data</strong> to see the dashboard run.
  </div>

  <div id="alert" class="alert ok">
    <svg width="17" height="17" viewBox="0 0 16 16" aria-hidden="true">
      <circle cx="8" cy="8" r="7" fill="none" stroke="currentColor" stroke-width="1.6"/>
      <path id="alertMark" d="M4.7 8.2 6.9 10.4 11.3 6" fill="none" stroke="currentColor" stroke-width="1.8" stroke-linecap="round" stroke-linejoin="round"/>
    </svg>
    <span id="alertText">Air clear</span>
    <span class="note" id="alertNote">MQ digital output — waiting for data</span>
  </div>

  <div class="filters">
    <span class="label">Window</span>
    <div class="seg" id="windowSeg" role="group" aria-label="Time window">
      <button data-ms="60000"  aria-pressed="false">1 min</button>
      <button data-ms="300000" aria-pressed="true">5 min</button>
      <button data-ms="900000" aria-pressed="false">15 min</button>
      <button data-ms="0"      aria-pressed="false">All</button>
    </div>
    <span class="label" id="sampleCount" style="margin-left:auto">No samples yet</span>
  </div>

  <section class="tiles">
    <div class="tile">
      <div class="name"><span class="swatch" style="background:var(--s-distance)"></span>Distance</div>
      <div class="value" id="vDist">—<span class="unit">cm</span></div>
      <div class="spark" id="sDist"></div>
    </div>
    <div class="tile">
      <div class="name"><span class="swatch" style="background:var(--s-gas)"></span>Gas level</div>
      <div class="value" id="vGas">—<span class="unit">/1023</span></div>
      <div class="spark" id="sGas"></div>
    </div>
    <div class="tile">
      <div class="name"><span class="swatch" style="background:var(--s-object)"></span>Object temp</div>
      <div class="value" id="vObj">—<span class="unit">°C</span></div>
      <div class="spark" id="sObj"></div>
    </div>
    <div class="tile">
      <div class="name"><span class="swatch" style="background:var(--s-ambient)"></span>Ambient temp</div>
      <div class="value" id="vAmb">—<span class="unit">°C</span></div>
      <div class="spark" id="sAmb"></div>
    </div>
  </section>

  <div class="card">
    <h2>Temperature</h2>
    <p class="cap">MLX90614 infrared sensor. Both series share one °C axis.</p>
    <div class="legend">
      <span class="item"><span class="swatch" style="background:var(--s-object)"></span>Object</span>
      <span class="item"><span class="swatch" style="background:var(--s-ambient)"></span>Ambient</span>
    </div>
    <div class="plot" id="pTemp"></div>
  </div>

  <div class="grid2">
    <div class="card">
      <h2>Distance</h2>
      <p class="cap">HC-SR04 ultrasonic, centimetres. Gaps are out-of-range echoes.</p>
      <div class="plot" id="pDist"></div>
    </div>
    <div class="card">
      <h2>Gas level</h2>
      <p class="cap">MQ sensor analog output, raw 10-bit ADC counts.</p>
      <div class="plot" id="pGas"></div>
    </div>
  </div>

  <div class="card">
    <h2>RFID scans</h2>
    <p class="cap">Cards presented to the RC522 reader.</p>
    <div class="tablewrap"><div class="scroll">
      <table>
        <thead><tr><th>Time</th><th>Card UID</th><th>Bytes</th></tr></thead>
        <tbody id="rfidBody"></tbody>
      </table>
    </div></div>
    <div class="empty" id="rfidEmpty">No cards scanned yet.</div>
  </div>

  <details>
    <summary>Table view — every reading as numbers</summary>
    <div class="card" style="margin-top:0">
      <div class="tablewrap"><div class="scroll">
        <table>
          <thead><tr>
            <th>Time</th><th>Distance (cm)</th><th>Gas (ADC)</th>
            <th>Gas digital</th><th>Object (°C)</th><th>Ambient (°C)</th>
          </tr></thead>
          <tbody id="dataBody"></tbody>
        </table>
      </div></div>
      <div class="empty" id="dataEmpty">No readings yet.</div>
    </div>
  </details>

  <details>
    <summary>Raw serial output</summary>
    <div class="console" id="rawConsole">Not connected.</div>
  </details>

</div>

<script>
"use strict";

/* ============================================================
   STATE
   ============================================================ */
const MAX_SAMPLES = 5000;
const samples = [];        // {t, dist, gas, gasD, obj, amb}
const rfid    = [];        // {t, uid}
let windowMs  = 300000;
let port = null, reader = null, pipePromise = null, demoTimer = null, stream = null;

const SERIES = {
  distance: { css: "--s-distance", name: "Distance", unit: "cm"    },
  gas:      { css: "--s-gas",      name: "Gas",      unit: ""      },
  obj:      { css: "--s-object",   name: "Object",   unit: "°C"    },
  amb:      { css: "--s-ambient",  name: "Ambient",  unit: "°C"    }
};
const col = k => getComputedStyle(document.documentElement).getPropertyValue(SERIES[k].css).trim();
const tok = n => getComputedStyle(document.documentElement).getPropertyValue(n).trim();
const $   = id => document.getElementById(id);

/* ============================================================
   SERIAL PARSER
   Reads the human-readable format main.ino already prints, so the
   sketch needs no changes. A block is committed when "Ambient Temp"
   arrives, since that is the last field the sketch emits.
   ============================================================ */
let pending = { dist: null, gas: null, gasD: null, obj: null, amb: null };

function parseLine(line) {
  const s = line.trim();
  if (!s) return;

  let m;
  if ((m = s.match(/^Distance:\s*(.+)$/i))) {
    pending.dist = /out of range/i.test(m[1]) ? null : parseFloat(m[1]);
  } else if ((m = s.match(/^Gas Analog:\s*(-?\d+)/i))) {
    pending.gas = parseInt(m[1], 10);
  } else if ((m = s.match(/^Gas Digital:\s*(-?\d+)/i))) {
    pending.gasD = parseInt(m[1], 10);
  } else if ((m = s.match(/^Object Temp:\s*(.+)$/i))) {
    pending.obj = /n\/a/i.test(m[1]) ? null : parseFloat(m[1]);
  } else if ((m = s.match(/^Ambient Temp:\s*(.+)$/i))) {
    pending.amb = /n\/a/i.test(m[1]) ? null : parseFloat(m[1]);
    commit();
  } else if ((m = s.match(/^UID:\s*([0-9A-Fa-f ]+)$/))) {
    rfid.unshift({ t: new Date(), uid: m[1].trim().toUpperCase() });
    if (rfid.length > 200) rfid.pop();
    renderRfid();
  }
}

function commit() {
  samples.push({
    t: new Date(),
    dist: pending.dist, gas: pending.gas, gasD: pending.gasD,
    obj: pending.obj,   amb: pending.amb
  });
  if (samples.length > MAX_SAMPLES) samples.shift();
  pending = { dist: null, gas: null, gasD: null, obj: null, amb: null };
  renderAll();
}

/* ============================================================
   WEB SERIAL
   ============================================================ */
function setStatus(text, cls) {
  $("statusText").textContent = text;
  $("status").className = "chip" + (cls ? " " + cls : "");
}

function logRaw(text) {
  const el = $("rawConsole");
  if (el.dataset.fresh !== "1") { el.textContent = ""; el.dataset.fresh = "1"; }
  el.textContent += text;
  if (el.textContent.length > 20000) el.textContent = el.textContent.slice(-16000);
  el.scrollTop = el.scrollHeight;
}

async function connect() {
  if (port) return disconnect();
  try {
    port = await navigator.serial.requestPort();
    await port.open({ baudRate: parseInt($("baud").value, 10) });
  } catch (err) {
    // Includes the user simply dismissing the port picker.
    if (err && err.name === "NotFoundError") { port = null; return; }
    setStatus("Open failed", "err");
    logRaw("\n[error] " + (err && err.message ? err.message : err) + "\n");
    port = null;
    return;
  }

  stopDemo();
  setStatus("Connected", "live");
  $("connect").textContent = "Disconnect";
  $("connect").classList.remove("primary");

  const decoder = new TextDecoderStream();
  pipePromise = port.readable.pipeTo(decoder.writable).catch(() => {});
  reader = decoder.readable.getReader();

  let buf = "";
  try {
    while (true) {
      const { value, done } = await reader.read();
      if (done) break;
      if (!value) continue;
      logRaw(value);
      buf += value;
      const lines = buf.split(/\r?\n/);
      buf = lines.pop();                 // keep the partial trailing line
      for (const line of lines) parseLine(line);
    }
  } catch (err) {
    logRaw("\n[read error] " + (err && err.message ? err.message : err) + "\n");
  }
  await disconnect();
}

async function disconnect() {
  try { if (reader) { await reader.cancel(); reader.releaseLock(); } } catch (e) {}
  try { if (pipePromise) await pipePromise; } catch (e) {}
  try { if (port) await port.close(); } catch (e) {}
  port = null; reader = null; pipePromise = null;
  setStatus("Disconnected", "");
  $("connect").textContent = "Connect Arduino";
  $("connect").classList.add("primary");
}

/* ============================================================
   WIFI - ESP32 bridge
   When the ESP32 serves this page, readings arrive as Server-Sent
   Events on port 81, one Arduino serial line per event, and go
   through the same parser as USB. ?esp=<ip> forces this mode, e.g.
   to test edits from localhost without re-flashing the ESP32.
   ============================================================ */
const STREAM_PORT = 81;

function espHost() {
  const forced = new URLSearchParams(location.search).get("esp");
  if (forced) return forced;
  // Plain http from a non-local host can only be the ESP32: GitHub
  // Pages is https, and local testing is localhost.
  const local = ["localhost", "127.0.0.1", "[::1]", ""];
  return location.protocol === "http:" && !local.includes(location.hostname)
    ? location.hostname : null;
}
const ESP_HOST = espHost();

function connectWifi() {
  if (stream) return disconnectWifi();
  stopDemo();
  setStatus("Connecting…", "");
  $("connect").textContent = "Disconnect";
  $("connect").classList.remove("primary");

  stream = new EventSource(`http://${ESP_HOST}:${STREAM_PORT}/`);
  stream.onopen = () => setStatus("Live over WiFi", "live");
  stream.onmessage = ev => { logRaw(ev.data + "\n"); parseLine(ev.data); };
  stream.onerror = () => {
    // CONNECTING means EventSource is already retrying on its own;
    // CLOSED means it gave up (e.g. the ESP32 refused: too many tabs).
    if (stream.readyState === EventSource.CONNECTING) {
      setStatus("Reconnecting…", "err");
    } else {
      logRaw("\n[stream closed by ESP32]\n");
      disconnectWifi();
      setStatus("Stream refused", "err");
    }
  };
}

function disconnectWifi() {
  if (stream) stream.close();
  stream = null;
  setStatus("Disconnected", "");
  $("connect").textContent = "Connect ESP32";
  $("connect").classList.add("primary");
}

/* ============================================================
   DEMO MODE - lets the page be verified with no hardware attached
   ============================================================ */
function stopDemo() {
  if (demoTimer) { clearInterval(demoTimer); demoTimer = null; $("demo").textContent = "Demo data"; }
}
function toggleDemo() {
  if (demoTimer) { stopDemo(); setStatus("Disconnected", ""); return; }
  if (port || stream) return;
  setStatus("Demo mode", "live");
  $("demo").textContent = "Stop demo";
  let k = 0;
  demoTimer = setInterval(() => {
    k++;
    const spike = Math.sin(k / 18) > 0.86;
    pending = {
      dist: k % 37 === 0 ? null : 18 + Math.sin(k / 7) * 11 + Math.random() * 1.6,
      gas:  Math.round(180 + Math.sin(k / 18) * 150 + Math.random() * 22),
      gasD: spike ? 0 : 1,
      obj:  31.5 + Math.sin(k / 11) * 4.2 + Math.random() * 0.5,
      amb:  24.4 + Math.sin(k / 40) * 1.1 + Math.random() * 0.25
    };
    commit();
    if (k % 23 === 0) {
      rfid.unshift({ t: new Date(), uid: "04 A3 " + (16 + (k % 200)).toString(16).toUpperCase().padStart(2,"0") + " C1" });
      renderRfid();
    }
  }, 1000);
}

/* ============================================================
   SCALES
   ============================================================ */
function niceScale(lo, hi, ticks) {
  if (!isFinite(lo) || !isFinite(hi)) return { lo: 0, hi: 1, step: 1 };
  if (lo === hi) { lo -= 1; hi += 1; }
  const raw  = (hi - lo) / ticks;
  const mag  = Math.pow(10, Math.floor(Math.log10(raw)));
  const norm = raw / mag;
  const step = (norm <= 1 ? 1 : norm <= 2 ? 2 : norm <= 5 ? 5 : 10) * mag;
  return { lo: Math.floor(lo / step) * step, hi: Math.ceil(hi / step) * step, step };
}
const hhmmss = d => d.toLocaleTimeString([], { hour12: false });
const visible = () => {
  if (!windowMs) return samples;
  const cut = Date.now() - windowMs;
  return samples.filter(s => s.t.getTime() >= cut);
};

/* ============================================================
   LINE CHART
   Solid hairline grid, 2px marks, direct end-labels in text ink with
   a colour chip carrying identity, crosshair + tooltip on hover.
   ============================================================ */
function lineChart(host, rows, keys, fmt) {
  const W = host.clientWidth || 600;
  const H = 210, PL = 46, PR = 62, PT = 12, PB = 30;
  const iw = Math.max(10, W - PL - PR), ih = H - PT - PB;

  let tip = host.querySelector(".tip");
  if (!tip) { tip = document.createElement("div"); tip.className = "tip"; host.appendChild(tip); }

  const pts = rows.filter(r => keys.some(k => r[k] != null && isFinite(r[k])));
  if (pts.length < 2) {
    host.querySelectorAll("svg").forEach(n => n.remove());
    if (!host.querySelector(".empty")) {
      const e = document.createElement("div");
      e.className = "empty"; e.textContent = "Waiting for readings…";
      host.appendChild(e);
    }
    return;
  }
  const ph = host.querySelector(".empty"); if (ph) ph.remove();

  let lo = Infinity, hi = -Infinity;
  for (const r of pts) for (const k of keys) {
    const v = r[k]; if (v == null || !isFinite(v)) continue;
    if (v < lo) lo = v; if (v > hi) hi = v;
  }
  const sc = niceScale(lo, hi, 4);
  const t0 = pts[0].t.getTime(), t1 = pts[pts.length - 1].t.getTime();
  const span = Math.max(1, t1 - t0);

  const X = t => PL + ((t - t0) / span) * iw;
  const Y = v => PT + ih - ((v - sc.lo) / (sc.hi - sc.lo)) * ih;

  let g = "";
  for (let v = sc.lo; v <= sc.hi + 1e-9; v += sc.step) {
    const y = Y(v);
    g += `<line x1="${PL}" y1="${y.toFixed(1)}" x2="${PL+iw}" y2="${y.toFixed(1)}" stroke="${tok('--gridline')}" stroke-width="1"/>`;
    g += `<text x="${PL-9}" y="${(y+4).toFixed(1)}" text-anchor="end" font-size="11" fill="${tok('--text-muted')}" style="font-variant-numeric:tabular-nums">${fmt(v)}</text>`;
  }
  g += `<line x1="${PL}" y1="${PT+ih}" x2="${PL+iw}" y2="${PT+ih}" stroke="${tok('--axis')}" stroke-width="1"/>`;

  const nT = Math.max(2, Math.min(5, Math.floor(iw / 96)));
  for (let i = 0; i <= nT; i++) {
    const t = t0 + (span * i) / nT, x = X(t);
    const anchor = i === 0 ? "start" : i === nT ? "end" : "middle";
    g += `<text x="${x.toFixed(1)}" y="${H-9}" text-anchor="${anchor}" font-size="11" fill="${tok('--text-muted')}" style="font-variant-numeric:tabular-nums">${hhmmss(new Date(t))}</text>`;
  }

  // Lines - break the path on null so gaps read as missing, not zero.
  for (const k of keys) {
    const c = col(k);
    let d = "", pen = false;
    for (const r of pts) {
      const v = r[k];
      if (v == null || !isFinite(v)) { pen = false; continue; }
      d += (pen ? "L" : "M") + X(r.t.getTime()).toFixed(1) + " " + Y(v).toFixed(1) + " ";
      pen = true;
    }
    g += `<path d="${d}" fill="none" stroke="${c}" stroke-width="2" stroke-linejoin="round" stroke-linecap="round"/>`;

    const last = [...pts].reverse().find(r => r[k] != null && isFinite(r[k]));
    if (last) {
      const x = X(last.t.getTime()), y = Y(last[k]);
      // 2px surface ring so overlapping end markers stay separable.
      g += `<circle cx="${x.toFixed(1)}" cy="${y.toFixed(1)}" r="4.5" fill="${c}" stroke="${tok('--surface-1')}" stroke-width="2"/>`;
      g += `<rect x="${(x+9).toFixed(1)}" y="${(y-5).toFixed(1)}" width="7" height="7" rx="1.5" fill="${c}"/>`;
      g += `<text x="${(x+20).toFixed(1)}" y="${(y+4).toFixed(1)}" font-size="12" font-weight="600" fill="${tok('--text-primary')}" style="font-variant-numeric:tabular-nums">${fmt(last[k])}</text>`;
    }
  }

  g += `<line id="cx" x1="0" y1="${PT}" x2="0" y2="${PT+ih}" stroke="${tok('--axis')}" stroke-width="1" opacity="0"/>`;
  g += `<rect x="${PL}" y="${PT}" width="${iw}" height="${ih}" fill="transparent"/>`;

  host.querySelectorAll("svg").forEach(n => n.remove());
  const svg = document.createElementNS("http://www.w3.org/2000/svg", "svg");
  svg.setAttribute("viewBox", `0 0 ${W} ${H}`);
  svg.setAttribute("height", H);
  svg.setAttribute("role", "img");
  svg.innerHTML = g;
  host.insertBefore(svg, tip);

  const cross = svg.querySelector("#cx");
  svg.addEventListener("mousemove", ev => {
    const box = svg.getBoundingClientRect();
    const px  = (ev.clientX - box.left) * (W / box.width);
    if (px < PL || px > PL + iw) { tip.style.opacity = 0; cross.setAttribute("opacity", 0); return; }
    const tAt = t0 + ((px - PL) / iw) * span;
    let best = pts[0], bd = Infinity;
    for (const r of pts) { const d = Math.abs(r.t.getTime() - tAt); if (d < bd) { bd = d; best = r; } }

    cross.setAttribute("x1", X(best.t.getTime()));
    cross.setAttribute("x2", X(best.t.getTime()));
    cross.setAttribute("opacity", 1);

    let html = `<div class="tt">${hhmmss(best.t)}</div>`;
    for (const k of keys) {
      const v = best[k];
      html += `<div class="row"><span class="swatch" style="background:${col(k)}"></span>`
           +  `${SERIES[k].name}<b>${v == null || !isFinite(v) ? "—" : fmt(v) + " " + SERIES[k].unit}</b></div>`;
    }
    tip.innerHTML = html;
    tip.style.opacity = 1;
    const sx = X(best.t.getTime()) * (box.width / W);
    tip.style.left = Math.min(Math.max(4, sx + 14), box.width - tip.offsetWidth - 4) + "px";
    tip.style.top  = "8px";
  });
  svg.addEventListener("mouseleave", () => { tip.style.opacity = 0; cross.setAttribute("opacity", 0); });
}

/* ============================================================
   SPARKLINE
   ============================================================ */
function spark(host, rows, key) {
  const W = host.clientWidth || 200, H = 34;
  const vals = rows.filter(r => r[key] != null && isFinite(r[key]));
  host.innerHTML = "";
  if (vals.length < 2) return;

  let lo = Infinity, hi = -Infinity;
  for (const r of vals) { if (r[key] < lo) lo = r[key]; if (r[key] > hi) hi = r[key]; }
  if (lo === hi) { lo -= 1; hi += 1; }

  const t0 = vals[0].t.getTime();
  const span = Math.max(1, vals[vals.length-1].t.getTime() - t0);
  const X = t => ((t - t0) / span) * (W - 6) + 3;
  const Y = v => H - 4 - ((v - lo) / (hi - lo)) * (H - 9);

  let d = "";
  vals.forEach((r, i) => { d += (i ? "L" : "M") + X(r.t.getTime()).toFixed(1) + " " + Y(r[key]).toFixed(1) + " "; });

  const c = col(key), last = vals[vals.length-1];
  host.innerHTML =
    `<svg viewBox="0 0 ${W} ${H}" height="${H}" aria-hidden="true">
       <path d="${d}" fill="none" stroke="${c}" stroke-width="2" stroke-linejoin="round" stroke-linecap="round"/>
       <circle cx="${X(last.t.getTime()).toFixed(1)}" cy="${Y(last[key]).toFixed(1)}" r="3.2" fill="${c}"/>
     </svg>`;
}

/* ============================================================
   RENDER
   ============================================================ */
const f1 = v => v.toFixed(1);
const f0 = v => Math.round(v).toString();

function setTile(id, v, fmt) {
  const el = $(id), unit = el.querySelector(".unit").outerHTML;
  el.innerHTML = (v == null || !isFinite(v) ? "—" : fmt(v)) + unit;
}

function renderAll() {
  const rows = visible();
  const last = samples[samples.length - 1];

  $("sampleCount").textContent =
    samples.length ? `${rows.length} shown · ${samples.length} total` : "No samples yet";

  if (last) {
    setTile("vDist", last.dist, f1);
    setTile("vGas",  last.gas,  f0);
    setTile("vObj",  last.obj,  f1);
    setTile("vAmb",  last.amb,  f1);

    const bad = last.gasD === 0;   // MQ digital output is active-LOW
    const al = $("alert");
    al.className = "alert " + (bad ? "critical" : "ok");
    al.style.color = bad ? "var(--st-critical)" : "var(--st-good)";
    $("alertText").textContent = bad ? "Gas detected" : "Air clear";
    $("alertText").style.color = "var(--text-primary)";
    $("alertMark").setAttribute("d", bad ? "M8 4.4V8.9 M8 11.4v.2" : "M4.7 8.2 6.9 10.4 11.3 6");
    $("alertNote").textContent = "MQ digital output · threshold set by the trimpot on the sensor board";
  }

  sparkKey($("sDist"), rows, "dist", "distance");
  sparkKey($("sGas"),  rows, "gas",  "gas");
  sparkKey($("sObj"),  rows, "obj",  "obj");
  sparkKey($("sAmb"),  rows, "amb",  "amb");

  chartKeys($("pTemp"), rows, [["obj","obj"],["amb","amb"]], f1);
  chartKeys($("pDist"), rows, [["dist","distance"]], f1);
  chartKeys($("pGas"),  rows, [["gas","gas"]], f0);

  renderTable(rows);
}

// The sample records use short field names; the SERIES map uses role
// names. These two shims keep the drawing code reading in role terms.
function sparkKey(host, rows, field, role) {
  spark(host, rows.map(r => ({ t: r.t, [role]: r[field] })), role);
}
function chartKeys(host, rows, pairs, fmt) {
  const mapped = rows.map(r => {
    const o = { t: r.t };
    for (const [field, role] of pairs) o[role] = r[field];
    return o;
  });
  lineChart(host, mapped, pairs.map(p => p[1]), fmt);
}

function renderTable(rows) {
  const body = $("dataBody");
  const recent = rows.slice(-200).reverse();
  $("dataEmpty").style.display = recent.length ? "none" : "";
  body.innerHTML = recent.map(r => `<tr>
      <td>${hhmmss(r.t)}</td>
      <td>${r.dist == null ? "—" : r.dist.toFixed(1)}</td>
      <td>${r.gas  == null ? "—" : r.gas}</td>
      <td>${r.gasD == null ? "—" : (r.gasD === 0 ? "gas" : "clear")}</td>
      <td>${r.obj  == null ? "—" : r.obj.toFixed(1)}</td>
      <td>${r.amb  == null ? "—" : r.amb.toFixed(1)}</td>
    </tr>`).join("");
}

function renderRfid() {
  const body = $("rfidBody");
  $("rfidEmpty").style.display = rfid.length ? "none" : "";
  body.innerHTML = rfid.map(r => `<tr>
      <td>${hhmmss(r.t)}</td>
      <td style="font-family:ui-monospace,Consolas,monospace">${r.uid}</td>
      <td>${r.uid.split(/\s+/).length}</td>
    </tr>`).join("");
}

/* ============================================================
   CSV
   ============================================================ */
function exportCsv() {
  if (!samples.length) return;
  const head = "timestamp,distance_cm,gas_adc,gas_digital,object_c,ambient_c\n";
  const body = samples.map(s => [
    s.t.toISOString(),
    s.dist == null ? "" : s.dist.toFixed(2),
    s.gas  == null ? "" : s.gas,
    s.gasD == null ? "" : s.gasD,
    s.obj  == null ? "" : s.obj.toFixed(2),
    s.amb  == null ? "" : s.amb.toFixed(2)
  ].join(",")).join("\n");

  const url = URL.createObjectURL(new Blob([head + body], { type: "text/csv" }));
  const a = document.createElement("a");
  a.href = url;
  a.download = "arduino-readings-" + new Date().toISOString().slice(0,19).replace(/[:T]/g,"-") + ".csv";
  a.click();
  URL.revokeObjectURL(url);
}

/* ============================================================
   WIRING
   ============================================================ */
$("connect").addEventListener("click", ESP_HOST ? connectWifi : connect);
$("demo").addEventListener("click", toggleDemo);
$("csv").addEventListener("click", exportCsv);

$("theme").addEventListener("click", () => {
  const dark = matchMedia("(prefers-color-scheme: dark)").matches;
  const cur  = document.documentElement.getAttribute("data-theme") || (dark ? "dark" : "light");
  document.documentElement.setAttribute("data-theme", cur === "dark" ? "light" : "dark");
  renderAll();
});

$("windowSeg").addEventListener("click", ev => {
  const b = ev.target.closest("button"); if (!b) return;
  windowMs = parseInt(b.dataset.ms, 10);
  [...ev.currentTarget.children].forEach(c => c.setAttribute("aria-pressed", String(c === b)));
  renderAll();
});

if (ESP_HOST) {
  // Web Serial and baud rate don't apply over WiFi.
  $("baud").hidden = true;
  $("sub").textContent = "Live readings streamed over WiFi from the ESP32 bridge — ultrasonic, gas, IR temperature and RFID.";
  connectWifi();
} else if (!("serial" in navigator)) {
  $("unsupported").classList.remove("hidden");
  $("connect").disabled = true;
}

addEventListener("resize", () => renderAll());
renderAll();
renderRfid();
</script>
</body>
</html>
)HTML";
