#include "WProgram.h"

struct dreg {
  unsigned char address;
  unsigned char value[4];
};

extern dreg g_registers[16];

// headers are stupid
void read_packet();
void read_s();
void read_n();
void read_p();
void read_pt();
void read_batch_data(unsigned char batch_length);
void read_data();
void read_checksum();

#define PT_COMMAND_FAILED   1     // 2^0,   bit    0
#define PT_HIDDEN           2     // 2^1,   bit    1
#define PT_BATCH_LENGTH     60    // 2^5:2, bits   2 thru 5
#define PT_BATCH            64    // 2^6,   bit    6
#define PT_DATA             128   // 2^7,   bit    7

// Registers
#define CREG_COM_SETTINGS          0
#define CREG_COM_RATES1            1
#define CREG_COM_RATES2            2
#define CREG_COM_RATES3            3
#define CREG_COM_RATES4            4
#define CREG_COM_RATES5            5
#define CREG_COM_RATES6            6
#define CREG_COM_RATES7            7
#define CREG_FILTER_SETTINGS       8
#define CREG_HOME_NORTH            9
#define CREG_HOME_EAST             10
#define CREG_HOME_UP               11
#define CREG_ZERO_PRESSURE         12
#define CREG_GYRO_TRIM_X           14
#define CREG_GYRO_TRIM_Y           15
#define CREG_GYRO_TRIM_Z           16
#define CREG_MAG_CAL1_1            66
#define CREG_MAG_CAL1_2            67
#define CREG_MAG_CAL1_3            68
#define CREG_MAG_CAL2_1            69
#define CREG_MAG_CAL2_2            70
#define CREG_MAG_CAL2_3            71
#define CREG_MAG_CAL3_1            72
#define CREG_MAG_CAL3_2            73
#define CREG_MAG_CAL3_3            74
#define CREG_MAG_BIAS_X            75
#define CREG_MAG_BIAS_Y            76
#define CREG_MAG_BIAS_Z            77
#define DREG_HEALTH                85
#define DREG_GYRO_RAW_XY           86
#define DREG_GYRO_RAW_Z            87
#define DREG_GYRO_TIME             88
#define DREG_ACCEL_RAW_XY          89
#define DREG_ACCEL_RAW_Z           90
#define DREG_ACCEL_TIME            91
#define DREG_MAG_RAW_XY            92
#define DREG_MAG_RAW_Z             93
#define DREG_MAG_RAW_TIME          94
#define DREG_PRESSURE_RAW          95
#define DREG_PRESSURE_TIME         96
#define DREG_TEMPERATURE_RAW1      97
#define DREG_TEMPERATURE_RAW2      98
#define DREG_TEMPERATURE_TIME      99
#define DREG_GYRO_PROC_X           100
#define DREG_GYRO_PROC_Y           101
#define DREG_GYRO_PROC_Z           102
#define DREG_GYRO_PROC_TIME        103
#define DREG_ACCEL_PROC_X          104
#define DREG_ACCEL_PROC_Y          105
#define DREG_ACCEL_PROC_Z          106
#define DREG_ACCEL_PROC_TIME       107
#define DREG_MAG_PROC_X            108
#define DREG_MAG_PROC_Y            109
#define DREG_MAG_PROC_Z            110
#define DREG_MAG_PROC_TIME         111
#define DREG_PRESSURE_PROC         112
#define DREG_PRESSURE_PROC_TIME    113
#define DREG_TEMPERATURE_PROC1     114
#define DREG_TEMPERATURE_PROC2     115
#define DREG_TEMPERATURE_PROC_TIME 116
#define DREG_QUAT_AB               117
#define DREG_QUAT_CD               118
#define DREG_QUAT_TIME             119
#define DREG_EULER_PHI_THETA       120
#define DREG_EULER_PSI             121
#define DREG_EULER_TIME            122
#define DREG_POSITION_NORTH        123
#define DREG_POSITION_EAST         124
#define DREG_POSITION_UP           125
#define DREG_POSITION_TIME         126
#define DREG_VELOCITY_NORTH        127
#define DREG_VELOCITY_EAST         128
#define DREG_VELOCITY_UP           129
#define DREG_VELOCITY_TIME         131
#define DREG_GPS_LATITUDE          132
#define DREG_GPS_LONGITUDE         133
#define DREG_GPS_ALTITUDE          134
#define DREG_GPS_COURSE            135
#define DREG_GPS_SPEED             136
#define DREG_GPS_TIME              137
#define DREG_GPS_DATE              138
#define DREG_GPS_SAT_1_2           139
#define DREG_GPS_SAT_3_4           140
#define DREG_GPS_SAT_5_6           141
#define DREG_GPS_SAT_7_8           142
#define DREG_GPS_SAT_9_10          143
#define DREG_GPS_SAT_11_12         144
#define DREG_GYRO_BIAS_X           145
#define DREG_GYRO_BIAS_Y           146
#define DREG_GYRO_BIAS_Z           147
#define DREG_BIAS_X_VARIANCE       148
#define DREG_BIAS_Y_VARIANCE       149
#define DREG_BIAS_Z_VARIANCE       150
#define DREG_QUAT_A_VARIANCE       151
#define DREG_QUAT_B_VARIANCE       152
#define DREG_QUAT_C_VARIANCE       153
#define DREG_QUAT_D_VARIANCE       154
#define GET_FW_REVISION            170
#define FLASH_COMMIT               171
#define RESET_TO_FACTORY           172
#define ZERO_GYROS                 173
#define SET_HOME_POSITION          174
#define RESET_EKF                  179

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

void setup_euler() {
   HWSERIAL.write('s');
   HWSERIAL.write('n');
   HWSERIAL.write('p');
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
    HWSERIAL.write('s');
    HWSERIAL.write('n');
    HWSERIAL.write('p');
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
    HWSERIAL.write('s');
    HWSERIAL.write('n');
    HWSERIAL.write('p');
    HWSERIAL.write(0);
    HWSERIAL.write(DREG_EULER_PHI_THETA);
    unsigned short checksum = (byte)'s' + (byte)'n' + (byte)'p' + DREG_EULER_PHI_THETA;
    byte checksum_buffer[2];
    checksum_buffer[0] = checksum >> 8;
    checksum_buffer[1] = checksum & 0x00ff;
    HWSERIAL.write(checksum_buffer, 2);
    HWSERIAL.flush(); 
}

// This really needs a way to pass a reference to the serial object
void read_packet() {
  read_s();
}

void read_s() {
  if(HWSERIAL.read() == 's')
    read_n();
}

void read_n() {
  if(HWSERIAL.read() == 'n')
    read_p();
}

void read_p() {
  if(HWSERIAL.read() == 'p')
    read_pt();
}

void read_pt() {
  unsigned char packet_type = HWSERIAL.read();
  if((packet_type & PT_COMMAND_FAILED) == PT_COMMAND_FAILED)
    return;

  if((packet_type & PT_DATA) == PT_DATA) { // has data
    if((packet_type & PT_BATCH) != PT_BATCH) { // non-batch
      read_data();
    } else { // batch
      unsigned char batch_length = ((packet_type & PT_BATCH_LENGTH) >> 2);
      read_batch_data(batch_length);
    }
  }
}

void read_batch_data(unsigned char batch_length) {
  unsigned char address = HWSERIAL.read();
  for(unsigned char i = 0; i < batch_length; i++) {
    HWSERIAL.readBytes(g_registers[i].value, 4);
    g_registers[i].address = address + i;
  }
  read_checksum();
}

void read_data() {
  g_registers[0].address = HWSERIAL.read();
  HWSERIAL.readBytes(g_registers[0].value, 4);
  read_checksum();
}

void read_checksum() {
  HWSERIAL.read();
  HWSERIAL.read();
}
