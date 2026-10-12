"""Copy index.html into the ESP32 sketch as dashboard_html.h.

The ESP32 serves the dashboard from its own flash, so it needs the page
compiled in. Re-run this after every edit to index.html, then re-upload
esp32_bridge:

    python embed_dashboard.py
"""

from pathlib import Path

ROOT = Path(__file__).resolve().parent
SOURCE = ROOT / "index.html"
TARGET = ROOT / "esp32_bridge" / "dashboard_html.h"

# Raw string literal delimiter. The page must never contain ')HTML"'
# or the C++ string would end early.
DELIM = "HTML"

html = SOURCE.read_text(encoding="utf-8")
if f'){DELIM}"' in html:
    raise SystemExit(f'index.html contains "){DELIM}\\"" - pick another DELIM')

TARGET.write_text(
    "// Generated from index.html by embed_dashboard.py - do not edit by hand.\n"
    "#pragma once\n"
    "\n"
    f'const char DASHBOARD_HTML[] PROGMEM = R"{DELIM}({html}){DELIM}";\n',
    encoding="utf-8",
    newline="\n",
)

print(f"Wrote {TARGET.relative_to(ROOT)} ({len(html.encode('utf-8'))} bytes)")
