# ESP32 IR Bridge: Hisense Remote → LG SH4D Soundbar

This ESP32-based project creates a simple infrared bridge between a **Hisense TV remote** (model: 55U7NQ) and a **LG SH4D soundbar**.

The goal is to control the soundbar using the TV remote only – seamlessly and wirelessly, without any modification to the remote.

## 🔧 How It Works

- The ESP32 receives IR signals from the Hisense remote.
- When a recognized command is detected (Power, Volume+, Volume–), the ESP32 maps it to the corresponding LG IR code.
- The code is **logged** (for now) – later, it can be transmitted via an IR LED (KY-005).
- A PWM-controlled RGB LED visualizes the state:
  - 🔴 Power
  - 🔵 Volume+
  - 🟢 Volume–
  - 🔹 Soft blue: Standby mode
- When holding a button, the color **pulses** using a repeated fade-in effect.

## 🛠 Hardware

| Component        | Description                              |
|------------------|------------------------------------------|
| ESP32            | WROOM module (e.g. DevKit)               |
| IR Receiver      | Keyes-compatible 38 kHz module (GPIO 23) |
| IR Transmitter   | KY-005 module (not yet active)           |
| RGB LED Module   | Common cathode, connected via PWM        |

## 📂 File Structure

.
├── sketch/
│ └── sketch.ino ← main code file
└── README.md ← project documentation


## 🧠 IR Code Mapping

| Hisense Code  | Action      | LG Code     |
|---------------|-------------|-------------|
| `0x00FDB04F`  | Power       | `0x34347887` |
| `0x00FD22DD`  | Volume Up   | `0x3434E817` |
| `0x00FDC23D`  | Volume Down | `0x34346897` |

Currently, the sketch **logs the LG code** that would be sent, but does **not yet transmit** it.

## 🧪 Compatibility

- ✅ Tested with:
  - Hisense **55U7NQ** TV remote
  - LG **SH4D** soundbar
  - ESP32 WROOM board
- 📦 IR transmitter (KY-005) is ordered, not active yet.

## ▶️ Next Steps

- [ ] Connect KY-005 to a PWM-capable pin (e.g., GPIO 17)
- [ ] Enable `IRsend.sendSamsung(...)` inside the sketch
- [ ] (Optional) Add long-press / debounce handling or Web UI

## 🔐 License

MIT License – use freely, modify proudly.

---

**Made with 🤖 + ❤️ by an Austrian software enthusiast**
