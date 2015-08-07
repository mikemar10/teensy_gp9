#define HWSERIAL Serial1
// Here there be dragons.  This is some Carmack inverse square root shit
#define MAGIC_SERVO_MIN 13107
#define MAGIC_SERVO_MAX 26214
#define DEBUG false
#define YAW 6
#define ROLL 9
#define PITCH 10
#include "gp9_util.h"

boolean packet_sent = false;


void setup() {
  if(DEBUG) {
   Serial.begin(9600);
  }
  HWSERIAL.begin(115200, SERIAL_8N1);

  analogWriteFrequency(6, 200);
  analogWriteFrequency(9, 200);
  analogWriteFrequency(10, 200);
  analogWriteResolution(16);

  delay(2000);

}

void loop() {
  if(!packet_sent) {
    if(DEBUG) {
     Serial.println("Zeroing out broadcasts, enabling euler, requesting yaw packet");
    }
    delay(1000);
    zero_out();
    //setup_euler();
    packet_sent = true;
  }
  
  read_packet();
  for(unsigned int i = 0; i < 16; i++) {
    switch(g_registers[i].address) {
      case DREG_EULER_PSI:
      case DREG_EULER_PHI_THETA:
      if(DEBUG) {
        Serial.print("Register: ");
        Serial.println(i);
        Serial.print("Address: ");
        Serial.println(g_registers[i].address);
        Serial.print("Data: ");
        for(unsigned int j = 0; j < 4; j++) {
          Serial.print(g_registers[i].value[j]);
          Serial.print(" ");
       }
      }
      if(g_registers[i].address == DREG_EULER_PSI) {
        short yaw = (g_registers[i].value[0] << 8) | (g_registers[i].value[1]);
        if(DEBUG) {
          Serial.println();
          Serial.print("Yaw: ");
          Serial.println((double)yaw / 5215.18917);
        }
        double yawRad = (double)yaw / 5215.18917;
        float yawDeg = -yawRad * 180 / 3.14159 + 195;
        long yaw_us = map(yawDeg * 10, 0, 1800, MAGIC_SERVO_MIN, MAGIC_SERVO_MAX);
        analogWrite(YAW, yaw_us);
      }
      if(g_registers[i].address == DREG_EULER_PHI_THETA) {
        short roll = (g_registers[i].value[0] << 8) | (g_registers[i].value[1]);
        if(DEBUG) {
          Serial.println();
          Serial.print("Roll: ");
          Serial.println((double)roll / 5215.18917);
        }
        double rollRad = (double)roll / 5215.18917;
        float rollDeg = -rollRad * 180 / 3.14159 + 105;
        long roll_us = map(rollDeg * 10, 0, 1800, MAGIC_SERVO_MIN, MAGIC_SERVO_MAX);
        analogWrite(ROLL, roll_us);
        
        short pitch = (g_registers[i].value[2] << 8) | (g_registers[i].value[3]);
        if(DEBUG) {
          Serial.print("Pitch: ");
          Serial.println((double)pitch / 5215.18917);
        }
        double pitchRad = (double)pitch / 5215.18917;
        float pitchDeg = -pitchRad * 180 / 3.14159 - 20;
        long pitch_us = map(pitchDeg * 10, 0, 1800, MAGIC_SERVO_MIN, MAGIC_SERVO_MAX);
        analogWrite(PITCH, pitch_us);
      }
      if(DEBUG) {
       Serial.println();
       Serial.println();
      }
      break;
    }
  }
  // clear registers
  for(unsigned int i = 0; i < 16; i++) {
    g_registers[i].address = 0;
    for(unsigned int j = 0; j < 4; j++) {
     g_registers[i].value[j] = 0;
    }
  }
  request_yaw();
  request_pitch_roll();
}

