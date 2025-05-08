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
