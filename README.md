# 🌱 Smart Greenhouse Controller — A Multi-Tool Engineering Journey

A single system concept — an autonomous greenhouse controller — carried through **five industry-standard engineering tools**: embedded control, analog circuit design, digital signal processing, virtual instrumentation, and professional PCB design. Built entirely in simulation, with **zero physical hardware**.

> 📄 **[Read the full engineering report (Word)](./Smart_Greenhouse_Controller_Full_Engineering_Report.docx)** — objectives, architecture, testing, debugging logs, and full source code for every stage below.

---

## The Journey

| Tool | What Was Built | Key Skill |
|---|---|---|
| **Tinkercad Circuits** | Smart Greenhouse Controller v2.1 — full embedded system | Embedded systems, sensors, actuators, PWM control |
| **LTspice** | RC Low-Pass Filter (following a regulator-design pivot) | Analog circuit simulation, frequency response analysis |
| **MATLAB** | 24-Hour Temperature Analysis dashboard | Digital filtering, statistics, data visualization |
| **LabVIEW** | Virtual Instrument Dashboard (gauges, LEDs, live chart) | Graphical programming, real-time monitoring |
| **KiCad** | Greenhouse Controller Shield — custom Arduino PCB | Schematic capture, ERC/DRC debugging, PCB routing, BOM |

---

## Part I — Tinkercad / Arduino: Embedded Control

A fully autonomous, closed-loop control system. Three analog sensors (temperature, light, soil moisture) drive five outputs — a variable-speed cooling fan, an irrigation servo valve, a grow light, an RGB status indicator, and an audible alarm — through a 7-state priority-based state machine.

**Highlights:** 5-sample sensor averaging · proportional PWM fan control · day/night-aware irrigation · serial telemetry & data logging. Two logic-ordering defects were found and fixed via a repeatable 6-scenario test protocol.

## Part II — LTspice: Power Supply & Filter Design

An initial 5V linear regulator design (7805 / LM317) hit missing SPICE models in LTspice's library. Rather than continuing to debug a tooling dead-end, the project pivoted to a fully characterized **RC low-pass filter** (1 kΩ / 1 µF, fc ≈ 159 Hz), verified at 100 Hz, 1 kHz, and 10 kHz — the same filtering principle protecting the Arduino's 5V rail in Part I.

## Part III — MATLAB: Sensor Data Analysis

24 hours of simulated greenhouse temperature data (1,441 samples), processed with a 10-point moving-average filter and analyzed against the same WARNING (28°C) / CRITICAL (35°C) thresholds used in the firmware. Results: 27.0°C average, 33.8°C peak, zero hours above critical.

## Part IV — LabVIEW: Virtual Instrument Dashboard

A graphical front panel — gauges, threshold-triggered LEDs, and a live waveform chart — reproducing the greenhouse's monitoring logic in LabVIEW's dataflow programming environment. Verified across a 20°C / 30°C / 40°C test sweep.

## Part V — KiCad: Custom PCB Design

A custom **Arduino Uno shield** — the Greenhouse Controller Shield — that replaces the breadboard from Part I entirely. Taken through the full professional workflow: schematic capture, Electrical Rule Check (36 → 3 → 0 errors), footprint assignment, PCB layout with a USB cutout, routing all 49 nets, Design Rule Check (0 errors, 0 warnings), 3D verification, and a full Bill of Materials.

---

## Repository Contents

```
├── greenhouse_controller.ino                                # Final Arduino firmware (v2.1)
├── Smart_Greenhouse_Controller_Full_Engineering_Report.docx  # Full 5-part engineering report
├── index.html                                                # Interactive project page (GitHub Pages)
└── README.md
```

## Author

**Abdulatef** — Final-Year BEng (Hons) Mechatronics Engineering, Kuala Lumpur, Malaysia
