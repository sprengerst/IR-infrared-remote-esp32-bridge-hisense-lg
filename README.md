# ESP32 IR Bridge: Hisense Remote → LG SH4D Soundbar

This ESP32-based project creates a simple infrared bridge between a **Hisense TV remote** (model: *55U7NQ*) and a **LG SH4D soundbar**.

The goal is to control the soundbar using the TV remote only – seamlessly and wirelessly, without any modification to the remote.

---

## 🔧 How It Works

- The ESP32 receives IR signals from the Hisense remote.
- When a recognized command is detected (Power, Volume+, Volume–, Mute), the ESP32 maps it to the corresponding LG IR code (Samsung protocol).
- The IR command is then sent via an IR LED (KY-005).
- A PWM-controlled RGB LED visualizes the command state:
  - 🔴 Power
  - 🔵 Volume+
  - 🟢 Volume–
  - 🟣 Mute
  - 🔹 Soft blue = standby
- While a button is held, the LED remains lit. After release, it fades back to standby.

---

## 🛠 Hardware

| Component        | Description                              |
|------------------|------------------------------------------|
| ESP32            | WROOM module (e.g. DevKit)               |
| IR Receiver      | Keyes-compatible 38 kHz module (GPIO 23) |
| IR Transmitter   | KY-005 module (GPIO 13, active)          |
| RGB LED Module   | Common cathode, connected via PWM        |

---

## 📂 File Structure

```
.
├── sketch/
│   ├── sketch.ino        ← main bridge code
│   └── receiver_test.ino ← debug tool to identify IR codes
└── README.md             ← this documentation
```

---

## 🧠 IR Code Mapping

| Hisense Remote  | Action      | LG Soundbar (Samsung protocol) |
|-----------------|-------------|-------------------------------|
| `0x00FDB04F`     | Power       | `0x34347887` (Power)          |
| `0x00FD22DD`     | Volume Up   | `0x3434E817` (Volume+)        |
| `0x00FDC23D`     | Volume Down | `0x34346897` (Volume–)        |
| `0xFD708F`       | Mute        | `0x3434F807` (Mute)           |

---

## 📡 receiver_test.ino (Optional Debug Sketch)

To help identify IR codes from a remote:

```cpp
#include <IRremoteESP8266.h>
#include <IRrecv.h>
#include <IRutils.h>

const uint16_t RECV_PIN = 23;  // Keyes receiver on GPIO 23
IRrecv irrecv(RECV_PIN);
decode_results results;

void setup() {
  Serial.begin(115200);
  irrecv.enableIRIn();  // Start the receiver
  Serial.println("IR Receiver Ready — Listening...");
}

void loop() {
  if (irrecv.decode(&results)) {
    Serial.println("----------");
    Serial.print("Protocol : ");
    Serial.println(typeToString(results.decode_type));  // e.g. LG, NEC, UNKNOWN

    Serial.print("Code     : 0x");
    Serial.println(resultToHexidecimal(&results));

    Serial.print("Bits     : ");
    Serial.println(results.bits);

    Serial.println("----------");

    irrecv.resume();  // Ready for next
  }
}
```

---

## ✅ Compatibility

Tested with:

- Hisense **55U7NQ** TV remote
- LG **SH4D** soundbar
- ESP32 WROOM DevKit
- KY-005 IR LED on GPIO 13
- Keyes IR Receiver on GPIO 23

---

## ▶️ Next Ideas

- [ ] Add web interface for status or Wi-Fi config
- [ ] Trigger long press / debounce handling
- [ ] Control multiple devices with mode switching

---

## 🔐 License

MIT License — use freely, modify proudly.

---

**Made with 🤖 + ❤️ by an Austrian software enthusiast**
