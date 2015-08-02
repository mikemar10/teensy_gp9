#include <Dynamixel_Serial.h>

#define HWSERIAL Serial1
#include "gp9_util.h"
#include <Wire.h>
#include <Servo.h>


// Servo constants
#define SERVO1_ID       0x01
#define SERVO2_ID       0x02
#define SERVO_PIN      0x02  // TODO: Change to proper pin - schultz
#define SERVO_BAUDRATE  57600
#define SERVO_CW_LIMIT  0x001
#define SERVO_CCW_LIMIT 0xFFF


boolean packet_sent = false;
Servo sroll;
Servo spitch;


void setup() {
  Serial.begin(9600);
  HWSERIAL.begin(115200, SERIAL_8N1);

  // Init servos
  Dynamixel.begin(SERVO_BAUDRATE, SERVO_PIN);
  Dynamixel.setMode(SERVO1_ID, SERVO, SERVO_CW_LIMIT, SERVO_CCW_LIMIT);
  Dynamixel.setMode(SERVO2_ID, SERVO, SERVO_CW_LIMIT, SERVO_CCW_LIMIT);

  sroll.attach(9);
  sroll.write(0);
  spitch.attach(10);
  spitch.write(0);

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
  
  read_packet();
  for(unsigned int i = 0; i < 16; i++) {
    switch(g_registers[i].address) {
     // case DREG_EULER_PSI:
      case DREG_EULER_PHI_THETA:
      Serial.print("Register: ");
      Serial.println(i);
      Serial.print("Address: ");
      Serial.println(g_registers[i].address);
      Serial.print("Data: ");
      for(unsigned int j = 0; j < 4; j++) {
        Serial.print(g_registers[i].value[j]);
        Serial.print(" ");
      }
      if(g_registers[i].address == DREG_EULER_PSI) {
        short yaw = (g_registers[i].value[0] << 8) | (g_registers[i].value[1]);
        Serial.println();
        Serial.print("Yaw: ");
        Serial.println((double)yaw / 5215.18917);
      }
      if(g_registers[i].address == DREG_EULER_PHI_THETA) {
        short roll = (g_registers[i].value[0] << 8) | (g_registers[i].value[1]);
        Serial.println();
        Serial.print("Roll: ");
        Serial.println((double)roll / 5215.18917);
        short pitch = (g_registers[i].value[2] << 8) | (g_registers[i].value[3]);
        Serial.print("Pitch: ");
        Serial.println((double)pitch / 5215.18917);
      }
      Serial.println();
      Serial.println();
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

