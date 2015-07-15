#include "WProgram.h"

struct GP9_Packet {
  char* snp;
  byte packet_type;
  byte register_address;
  byte* data_bytes;
  unsigned short checksum;
};

byte CREG_COM_SETTINGS    = 0x00;
byte CREG_COM_RATES1      = 0x01;
byte CREG_COM_RATES2      = 0x02;
byte CREG_COM_RATES3      = 0x03;
byte CREG_COM_RATES4      = 0x04;
byte CREG_COM_RATES5      = 0x05;
byte CREG_COM_RATES6      = 0x06;
byte CREG_COM_RATES7      = 0x07;
byte CREG_FILTER_SETTINGS = 0x08;
byte CREG_HOME_NORTH      = 0x09;
byte CREG_HOME_EAST       = 0x0A;
byte CREG_HOME_UP         = 0x0B;
byte CREG_ZERO_PRESSURE   = 0x0C;
byte CREG_GYRO_TRIM_X     = 0x0E;
byte CREG_GYRO_TRIM_Y     = 0x0F;
byte CREG_GYRO_TRIM_Z     = 0x10;                                                                                                                                                                                                             
byte CREG_MAG_CAL1_1      = 0x42;
byte CREG_MAG_CAL1_2      = 0x43;
byte CREG_MAG_CAL1_3      = 0x44;
byte CREG_MAG_CAL2_1      = 0x45;
byte CREG_MAG_CAL2_2      = 0x46;
byte CREG_MAG_CAL2_3      = 0x47;
byte CREG_MAG_CAL3_1      = 0x48;
byte CREG_MAG_CAL3_2      = 0x49;
byte CREG_MAG_CAL3_3      = 0x4A;
byte CREG_MAG_BIAS_X      = 0x4B;
byte CREG_MAG_BIAS_Y      = 0x4C;
byte CREG_MAG_BIAS_Z      = 0x4D;
byte DREG_EULER_PHI_THETA = 0x78;
byte DREG_EULER_PSI       = 0x79;
byte DREG_EULER_TIME      = 0x7A;

byte P_HAS_DATA = 0x80;
byte P_IS_BATCH = 0x40;

GP9_Packet packet_factory(byte p_type, byte address, byte* data) {
  GP9_Packet p = {
    (char*)"snp",
    p_type,
    address,
    data,
    0
  };
  
  p.checksum = (byte)'s' + (byte)'n' + (byte)'p' + p_type + address;
  for(unsigned int i = 0; i < sizeof(data); i++) {
    p.checksum += data[i];
  }
  return p;
}
void zero_out() {
 // { 0x73, 0x6e, 0x70, 0x80, i, 0, 0, 0, 0, 
  for(byte i = 0x01; i <= 0x07; i += 0x01) {
   HWSERIAL.write('s');
   HWSERIAL.write('n');
   HWSERIAL.write('p');
   HWSERIAL.write(0x80);
   HWSERIAL.write(i);
   HWSERIAL.write(0);
   HWSERIAL.write(0);
   HWSERIAL.write(0);
   HWSERIAL.write(0);
   unsigned short checksum = (byte)'s' + (byte)'n' + (byte)'p' + 0x80 + i;
   byte checksum_buffer[2];
   checksum_buffer[0] = checksum >> 8;
   checksum_buffer[1] = checksum & 0x00ff;
   HWSERIAL.write(checksum_buffer, 2);
   HWSERIAL.flush();
  }
}

void snp() {
   HWSERIAL.write('s');
   HWSERIAL.write('n');
   HWSERIAL.write('p');
}

void setup_euler() {
   snp();
   HWSERIAL.write(0x80);
   HWSERIAL.write(0x05);
   HWSERIAL.write(0);
   HWSERIAL.write(0xff);
   HWSERIAL.write(0);
   HWSERIAL.write(0);
   unsigned short checksum = (byte)'s' + (byte)'n' + (byte)'p' + 0x80 + 0x05 + 0xff;
   byte checksum_buffer[2];
   checksum_buffer[0] = checksum >> 8;
   checksum_buffer[1] = checksum & 0x00ff;
   HWSERIAL.write(checksum_buffer, 2);
   HWSERIAL.flush();
}

void request_yaw() {
    snp();
    HWSERIAL.write(0);
    HWSERIAL.write(DREG_EULER_PSI);
    unsigned short checksum = (byte)'s' + (byte)'n' + (byte)'p' + DREG_EULER_PSI;
    byte checksum_buffer[2];
    checksum_buffer[0] = checksum >> 8;
    checksum_buffer[1] = checksum & 0x00ff;
    HWSERIAL.write(checksum_buffer, 2);
    HWSERIAL.flush(); 
}

void request_pitch_roll() {
    snp();
    HWSERIAL.write(0);
    HWSERIAL.write(DREG_EULER_PHI_THETA);
    unsigned short checksum = (byte)'s' + (byte)'n' + (byte)'p' + DREG_EULER_PHI_THETA;
    byte checksum_buffer[2];
    checksum_buffer[0] = checksum >> 8;
    checksum_buffer[1] = checksum & 0x00ff;
    HWSERIAL.write(checksum_buffer, 2);
    HWSERIAL.flush(); 
}

void request_euler_time() {
    snp();
    HWSERIAL.write(0);
    HWSERIAL.write(DREG_EULER_TIME);
    unsigned short checksum = (byte)'s' + (byte)'n' + (byte)'p' + DREG_EULER_TIME;
    byte checksum_buffer[2];
    checksum_buffer[0] = checksum >> 8;
    checksum_buffer[1] = checksum & 0x00ff;
    HWSERIAL.write(checksum_buffer, 2);
    HWSERIAL.flush(); 
}
