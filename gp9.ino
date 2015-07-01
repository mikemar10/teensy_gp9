#define HWSERIAL Serial1
#include "gp9_util.h"
 boolean packet_sent = false;

void setup() {
  Serial.begin(9600);
  HWSERIAL.begin(115200, SERIAL_8N1);
  
  delay(2000);
 
}

void loop() {
  if(!packet_sent) {
    Serial.println("Zeroing out broadcasts, enabling euler, requesting yaw packet");
    delay(1000);
    zero_out();
    //setup_euler();
    packet_sent = true;
  }
    while(HWSERIAL.available() > 0 && HWSERIAL.read() == 's' && HWSERIAL.read() == 'n' && HWSERIAL.read() == 'p') {
    Serial.println();
    byte p_type = HWSERIAL.read();
    Serial.print("Packet type: ");
    Serial.print(p_type, HEX);
    Serial.println();
    Serial.print("Address: ");
    Serial.print(HWSERIAL.read(), HEX);
    Serial.println();
    if (p_type & 0x80) { //has data
      Serial.println("PACKET HAS DATA");
      if (p_type & 0x40) { //batch
        Serial.println("BATCH PACKET");
        byte b_length = (p_type & 0x3C) >> 2;
        Serial.print("Batch length: ");
        Serial.print(b_length, DEC);
        Serial.println();
        byte p_buffer[b_length * 4];
        Serial.print("Hex buffer: ");
        for(unsigned int i = 0; i < b_length * 4; i++) {
          p_buffer[i] = HWSERIAL.read();
          Serial.print(" ");
          Serial.print(p_buffer[i], HEX);
          Serial.print(" ");
        }
        Serial.println();
      } else {
        Serial.print("Hex buffer: ");
        byte p_buffer[4];
        for(unsigned int i = 0; i < 4; i++) {
          p_buffer[i] = HWSERIAL.read();
          Serial.print(" ");
          Serial.print(p_buffer[i], HEX);
          Serial.print(" ");
        }
        short yaw = 0;
        yaw = p_buffer[0] << 8;
        yaw = yaw ^ p_buffer[1];
        Serial.print("Yaw: ");
        Serial.print((double)yaw / 5215.18917);
        Serial.println();
      }
    }

    Serial.println();
    Serial.print("Checksum: ");
    unsigned short checksum = 0;
    checksum = HWSERIAL.read() << 8;
    checksum = checksum ^ HWSERIAL.read();
    Serial.print(checksum, HEX);
    Serial.println();
    Serial.flush();
    }
    delay(50);
    request_yaw();
}

