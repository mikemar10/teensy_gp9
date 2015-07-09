#define HWSERIAL Serial1
#include "gp9_util.h"
#include <Wire.h>
#include <Servo.h>
boolean packet_sent = false;
Servo sroll;
Servo spitch;

void setup() {
  HWSERIAL.begin(115200, SERIAL_8N1);

  sroll.attach(9);
  sroll.write(0);
  spitch.attach(10);
  spitch.write(0);

  delay(2000);

}

void loop() {
  if(!packet_sent) {
    delay(1000);
    zero_out();
    //setup_euler();
    packet_sent = true;
  }
  while(HWSERIAL.available() > 0 && HWSERIAL.read() == 's' && HWSERIAL.read() == 'n' && HWSERIAL.read() == 'p') {
    byte p_type = HWSERIAL.read();
    byte address = HWSERIAL.read();
    if (p_type & 0x80) { //has data
      if (p_type & 0x40) { //batch
        byte b_length = (p_type & 0x3C) >> 2;
        byte p_buffer[b_length * 4];
        for(unsigned int i = 0; i < b_length * 4; i++) {
          p_buffer[i] = HWSERIAL.read();
        }
      } else {
        byte p_buffer[4];
        for(unsigned int i = 0; i < 4; i++) {
          p_buffer[i] = HWSERIAL.read();
        }
        if(address == 0x79) {
          short yaw = 0;
          yaw = p_buffer[0] << 8;
          yaw = yaw ^ p_buffer[1];
        } else if(address == 0x78) {
          short roll = 0;
          short pitch = 0;
          roll = p_buffer[0] << 8;
          roll = roll ^ p_buffer[1];
          pitch = p_buffer[2] << 8;
          pitch = pitch ^ p_buffer[3];

          double pitchRad = (double)pitch / 5215.18917;
          float pitchDeg = (pitchRad * 180 / 3.14159 + 180) - 90;
          spitch.write(pitchDeg);

          double rollRad = (double)roll / 5215.18917;
          float rollDeg = (rollRad * 180 / 3.14159 + 180) - 90;
          sroll.write(rollDeg);
        }
      }
    }

    unsigned short checksum = 0;
    checksum = HWSERIAL.read() << 8;
    checksum = checksum ^ HWSERIAL.read();
  }
  delay(50);
  //request_yaw();
  request_pitch_roll();
}

