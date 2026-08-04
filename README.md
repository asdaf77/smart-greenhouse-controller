# 🌱 Smart Greenhouse Controller

A fully autonomous, closed-loop embedded control system that monitors environmental conditions and drives real-world outputs in response — designed, wired, programmed, tested, and debugged **entirely in a browser-based circuit simulator**, with zero physical hardware.

> 📄 **[Read the full project report (Word)](./Smart_Greenhouse_Controller_Project_Report.docx)** — executive summary, circuit design, testing protocol, debugging log, and full source code.

---

## Overview

The controller reads **temperature, ambient light, and soil moisture** through three analog sensors, filters the readings with multi-sample averaging, and drives **five independent outputs** — a variable-speed cooling fan, an irrigation servo valve, a grow light, an RGB status indicator, and an audible alarm — through a priority-based state machine. A 16×2 LCD and the serial console provide live and historical visibility into system behavior.

**Tools used:** Tinkercad Circuits · Arduino C/C++ · `LiquidCrystal.h` · `Servo.h`

## Key Features

- **Sensor averaging** — 5-sample averaging per sensor to suppress electrical noise
- **Proportional (PWM) fan control** — fan speed scales with temperature (28–40 °C → PWM 100–255) instead of simple on/off
- **Day/night-aware irrigation** — the valve only opens when soil is dry **and** it's daytime; at night it enters a distinct "waiting" state
- **7-state priority display** — `ALERT!` → `HOT!` → `COOLING` → `WATERING` → `WAIT 4 MORN` → `GROWING` → `OPTIMAL`
- **RGB + buzzer alerting** — green / amber / red-with-alarm severity tiers
- **Serial telemetry & data logging** — live tabulated readings plus a rolling min/max report every 10 seconds

## Components

| Component | Role |
|---|---|
| Arduino Uno R3 | Central controller |
| TMP36 | Temperature sensing |
| Photoresistor + 10 kΩ resistor | Light sensing (voltage divider) |
| Potentiometer | Simulated soil-moisture sensor |
| Potentiometer | LCD contrast control |
| 16×2 LCD | Live readings & system status |
| DC motor | Cooling fan (PWM) |
| Micro servo | Irrigation valve |
| LED | Grow light |
| RGB LED | Status indicator |
| Buzzer | Critical alarm |

## Debugging Highlights

Two logic-ordering defects were found and fixed through a repeatable 6-scenario test protocol (see full report for details):

1. **`WAIT 4 MORN` never displayed** — the priority ladder checked the grow-light condition before the night-irrigation-wait condition. Fixed by reordering priorities.
2. **Fan PWM response was barely visible** — the temperature-to-PWM mapping spanned too wide a range. Fixed by narrowing the active range and raising the output floor.

## Repository Contents

```
├── greenhouse_controller.ino   # Final, debugged firmware (v2.1)
├── Smart_Greenhouse_Controller_Project_Report.docx   # Full project report
└── README.md
```

## Author

**Abdulatef** — Final-Year BEng (Hons) Mechatronics Engineering, Asia Pacific University of Technology and Innovation (APU), Kuala Lumpur
