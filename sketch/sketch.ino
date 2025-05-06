#include <IRremoteESP8266.h>
#include <IRrecv.h>
#include <IRutils.h>

const uint16_t RECV_PIN = 23;
IRrecv irrecv(RECV_PIN);
decode_results results;

// RGB Pins + PWM Kanäle
const uint8_t PIN_R = 26, PIN_G = 27, PIN_B = 25;
const uint8_t CH_R = 0, CH_G = 1, CH_B = 2;
const uint16_t PWM_FREQ = 5000;
const uint8_t PWM_RES = 8;

// IR-Codes
#define HISENSE_PWR  0x00FDB04F
#define HISENSE_VOLP 0x00FD22DD
#define HISENSE_VOLM 0x00FDC23D
#define NEC_REPEAT   0xFFFFFFFF

// LG-Zielcodes
#define LG_PWR   0x34347887
#define LG_VOLP  0x3434E817
#define LG_VOLM  0x34346897

// Farben
#define STBY_R 0
#define STBY_G 0
#define STBY_B 6

#define PWR_R 255
#define PWR_G 0
#define PWR_B 0

#define VOLP_R 0
#define VOLP_G 180
#define VOLP_B 255

#define VOLM_R 0
#define VOLM_G 255
#define VOLM_B 0

enum FadeState { IDLE, FADE_UP, HOLD, FADE_DOWN, TO_STANDBY };
FadeState fadeState = IDLE;

uint8_t targetR = STBY_R, targetG = STBY_G, targetB = STBY_B;
uint8_t valueR = STBY_R, valueG = STBY_G, valueB = STBY_B;

unsigned long fadeTimer = 0;
const uint16_t fadeStepTime = 1;
const uint16_t fadeHoldTime = 400;

uint32_t lastCode = 0;

void setupPWM() {
  ledcSetup(CH_R, PWM_FREQ, PWM_RES); ledcAttachPin(PIN_R, CH_R);
  ledcSetup(CH_G, PWM_FREQ, PWM_RES); ledcAttachPin(PIN_G, CH_G);
  ledcSetup(CH_B, PWM_FREQ, PWM_RES); ledcAttachPin(PIN_B, CH_B);
  ledcWrite(CH_R, valueR);
  ledcWrite(CH_G, valueG);
  ledcWrite(CH_B, valueB);
}

void setColor(uint8_t r, uint8_t g, uint8_t b, FadeState nextState = FADE_UP) {
  targetR = r;
  targetG = g;
  targetB = b;
  valueR = 0;
  valueG = 0;
  valueB = 0;
  fadeTimer = millis();
  fadeState = nextState;
}

void returnToStandby() {
  targetR = STBY_R;
  targetG = STBY_G;
  targetB = STBY_B;
  valueR = 0;
  valueG = 0;
  valueB = 0;
  fadeState = TO_STANDBY;
  fadeTimer = millis();
}

void updateChannel(uint8_t ch, uint8_t &val, uint8_t target) {
  if (val < target) val++;
  else if (val > target) val--;
  ledcWrite(ch, val);
}

bool channelsCloseEnough(uint8_t v1, uint8_t t1, uint8_t v2, uint8_t t2, uint8_t v3, uint8_t t3) {
  return (abs((int)v1 - (int)t1) <= 2 &&
          abs((int)v2 - (int)t2) <= 2 &&
          abs((int)v3 - (int)t3) <= 2);
}

void processFade() {
  static unsigned long lastStepTime = 0;
  unsigned long now = millis();
  if (now - lastStepTime < fadeStepTime) return;
  lastStepTime = now;

  switch (fadeState) {
    case FADE_UP:
      updateChannel(CH_R, valueR, targetR);
      updateChannel(CH_G, valueG, targetG);
      updateChannel(CH_B, valueB, targetB);
      if (channelsCloseEnough(valueR, targetR, valueG, targetG, valueB, targetB)) {
        fadeState = HOLD;
        fadeTimer = now;
      }
      break;

    case HOLD:
      if (now - fadeTimer >= fadeHoldTime) {
        fadeState = FADE_DOWN;
        fadeTimer = now;
      }
      break;

    case FADE_DOWN:
      updateChannel(CH_R, valueR, 0);
      updateChannel(CH_G, valueG, 0);
      updateChannel(CH_B, valueB, 0);
      if (channelsCloseEnough(valueR, 0, valueG, 0, valueB, 0)) {
        returnToStandby();
      }
      break;

    case TO_STANDBY:
      updateChannel(CH_R, valueR, targetR);
      updateChannel(CH_G, valueG, targetG);
      updateChannel(CH_B, valueB, targetB);
      if (channelsCloseEnough(valueR, targetR, valueG, targetG, valueB, targetB)) {
        fadeState = IDLE;
      }
      break;

    case IDLE:
      break;
  }
}

void logBridgedSend(uint32_t code) {
  if (code == HISENSE_PWR) {
    Serial.println("→ Sende LG-Code: 0x34347887 (Power)");
  } else if (code == HISENSE_VOLP) {
    Serial.println("→ Sende LG-Code: 0x3434E817 (Volume+)");
  } else if (code == HISENSE_VOLM) {
    Serial.println("→ Sende LG-Code: 0x34346897 (Volume–)");
  }
}

void handleCode(uint32_t code) {
  lastCode = code;

  uint8_t r, g, b;
  if (code == HISENSE_PWR) {
    r = PWR_R; g = PWR_G; b = PWR_B;
  } else if (code == HISENSE_VOLP) {
    r = VOLP_R; g = VOLP_G; b = VOLP_B;
  } else if (code == HISENSE_VOLM) {
    r = VOLM_R; g = VOLM_G; b = VOLM_B;
  } else {
    return;
  }

  logBridgedSend(code); // <-- Logging der Brücke
  setColor(r, g, b, FADE_UP);
}

void setup() {
  Serial.begin(115200);
  irrecv.enableIRIn();
  setupPWM();
  Serial.println("Ready — Hisense to LG bridge logging active");
}

void loop() {
  processFade();

  if (irrecv.decode(&results)) {
    uint32_t code = results.value;
    irrecv.resume();

    if (code == NEC_REPEAT) {
      handleCode(lastCode);
    } else {
      handleCode(code);
    }
  }
}
