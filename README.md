# Wata — Modular-Adaptive Incubator for Microbiological Water Quality Monitoring

**Wata** is a low-cost, open-source, modular-adaptive bacteriological incubator designed for microbiological water quality monitoring in rural and resource-constrained settings. It was developed by the Cooperation Group on Water and Sanitation for Development (ASADE) at Universidad Politécnica de Madrid (UPM) in collaboration with Universidad de Costa Rica (UCR).

---

## 📦 Repository Contents

```
wata-incubator/
├── firmware/
│   ├── wata_config_21_manual.ino       # Configuration 2.1 — Manual control (buttons + OLED)
│   ├── wata_config_22_iot.ino          # Configuration 2.2 — IoT/Wi-Fi control (Blynk app)
│   └── wata_menu_incubadora.ino        # Incubator menu interface
├── README.md
└── LICENSE
```

---

## 🔧 System Description

Wata is structured around **three functional modules**, each available in two alternative configurations:

| Module | Configuration A | Configuration B |
|---|---|---|
| **Isolation (Module 1)** | Cork insulation (1.1) | Expanded polystyrene (1.2) |
| **Control (Module 2)** | Manual interface — buttons + OLED (2.1) | IoT/Wi-Fi — Blynk app (2.2) |
| **Energy (Module 3)** | Grid-connected + batteries (3.1) | Photovoltaic system + batteries (3.2) |

The combination of these modules yields **8 possible system configurations**, allowing communities and practitioners to select the arrangement best suited to their local technical, economic, and environmental conditions.

---

## 🖥️ Firmware

### Configuration 2.1 — Manual Control (`wata_config_21_manual.ino`)
Local user interface based on push buttons, LEDs and an OLED display. Designed for off-grid or low-connectivity contexts where no smartphone or Wi-Fi access is available. Temperature is regulated automatically via PWM control on the ESP8266 microcontroller.

### Configuration 2.2 — IoT/Wi-Fi Control (`wata_config_22_iot.ino`)
Remote monitoring and data management through the **Blynk** IoT platform. Enables real-time temperature tracking, cloud data storage, and automated email alerts. Recommended for contexts with stable Wi-Fi and mobile device availability.

### Incubator Menu (`wata_menu_incubadora.ino`)
User interaction menu displayed on the OLED screen, guiding the operator through the incubation process step by step. Supports both E. coli analysis (44 °C) and total coliform analysis (37 °C).

---

## ⚙️ Hardware Requirements

- NodeMCU ESP8266 ESP-12E microcontroller
- DHT22 temperature and humidity sensor
- OLED display 128×64 px (I2C, SSH1106)
- Low-voltage silicone rubber heating cable (5 Ω/m)
- PC817 optocoupler + IRF530 MOSFET transistor (power electronics)
- Push buttons × 2, LED indicators × 4
- Recycled aluminum cans (heating support + sample holder)
- Plastic enclosure (hermetic container GN 1/6, 2.6 L)
- Insulation material: natural cork (Config. 1.1) or expanded polystyrene (Config. 1.2)
- Energy supply: USB power bank 26800 mAh and/or 50W monocrystalline solar panel + PWM charge controller

---

## 📚 Required Arduino Libraries

```cpp
#include <Wire.h>
#include <SSD1306Wire.h>       // or SSD1306.h
#include <DHT.h>
#include <SPI.h>
#include <ESP8266WiFi.h>
#include <SimpleTimer.h>
#include <TimeLib.h>
// Config 2.2 only:
#include <BlynkSimpleEsp8266.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
```

Install via Arduino IDE → Library Manager or download from their respective repositories.

---

## 🌡️ Performance

Laboratory validation demonstrated:
- Temperature stability at 37 °C with deviations **< ±0.5 °C**
- Preheating time: **45–65 min**
- Recovery time after door opening: **< 10 min**
- Compliant with **ISO 9308-1 / UNE-ISO 9308-1:2014** requirements (36 ± 2 °C)

---

## 💶 Approximate Material Cost

| Configuration | Approximate cost |
|---|---|
| Grid-connected (Module 3.1) | ~107 € |
| Photovoltaic (Module 3.2) | ~186 € |

Costs correspond to August 2023 prices and may vary by country and supplier.

---

## 📖 Citation

If you use this design or firmware in your work, please cite:

> Núñez, R., Rivera-Montero, L., Vargas, E., Merayo, N. (2025). *A low-cost and open-source microbiological incubator for water quality assessment in isolated rural areas*. [Journal name]. https://github.com/raquel-nunez/wata-incubator

---

## 👩‍🔬 Authors

- **Raquel Núñez** — Universidad Politécnica de Madrid (UPM) · raquel.nunez@upm.es
- **Luis Rivera-Montero** — Universidad de Costa Rica (UCR)
- **Eugenia Vargas** — Universidad de Costa Rica (UCR)
- **Noemí Merayo** — Universidad Politécnica de Madrid (UPM)

---

## 📄 License

This project is licensed under the **GNU General Public License v3.0** — see the [LICENSE](LICENSE) file for details. You are free to use, modify, and distribute this work for non-commercial purposes with attribution.

---

## 🤝 Acknowledgements

This work was developed within the framework of the UPM Programme for Final Degree Projects in International Cooperation for Sustainable Development (TFT-CIDS), with field implementation supported by Universidad de Costa Rica in rural communities of Abangares, Costa Rica.
