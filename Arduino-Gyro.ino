// Connect GND, S1 and SR pins together.

#include <Wire.h>

// Radio
#include <nRF24L01.h>
#include <RF24.h>

RF24 radio(9, 8);  // Create radio object with RF24 library

float Yaw, Roll, Pitch, magx, magy, magz, accx, accy, accz, gyrox, gyroy, gyroz, q0, q1, q2, q3, Roll2, Pitch2, Yaw2, LIAx, LIAy, LIAz, GRVx, GRVy, GRVz;
const int GY_955 = 0x29;


void setup() {
  radio.begin();  // Init radio
  radio.setChannel(0xA);  // Set data transmition channel (from 0 to 127)
  radio.setDataRate(RF24_1MBPS);  // Transmition speed (should be the same on both devices)
  radio.setPALevel(RF24_PA_MAX);  // Transmition power
  radio.openWritingPipe(0x0123456789LL);  // Open tube for data transmition (one tube one transmitter)
  
  Wire.begin();
  Wire.setClock(400000);  // I2C clock rate, You can delete it but it helps the speed of I2C (default rate is 100000 Hz)
  delay(100);
  Wire.beginTransmission(GY_955);
  Wire.write(0x3E);  // Power Mode 
  Wire.write(0x00);  // Normal:0X00 (or B00), Low Power: 0X01 (or B01) , Suspend Mode: 0X02 (orB10)
  Wire.endTransmission();
  delay(100);
  Wire.beginTransmission(GY_955);
  Wire.write(0x3D);  // Operation Mode
  Wire.write(0x0C);  // NDOF:0X0C (or B1100) , IMU:0x08 (or B1000) , NDOF_FMC_OFF: 0x0B (or B1011)
  Wire.endTransmission();
  delay(100);
  
  Serial.begin(115200);  // Setting the baudrate
  delay(100);
}


void loop() {
  Wire.beginTransmission(GY_955);
  Wire.write(0x08);  
  Wire.endTransmission(false);
  Wire.requestFrom(GY_955,32,true);
  
  // Accelerometer
  accx = (int16_t)(Wire.read()|Wire.read()<<8 )/100.00; // m/s^2
  accy = (int16_t)(Wire.read()|Wire.read()<<8 )/100.00; // m/s^2
  accz = (int16_t)(Wire.read()|Wire.read()<<8 )/100.00; // m/s^2
  
  // Magnetometer
  magx = (int16_t)(Wire.read()|Wire.read()<<8 ) / 16.00; // mT
  magy = (int16_t)(Wire.read()|Wire.read()<<8 ) / 16.00; // mT
  magz = (int16_t)(Wire.read()|Wire.read()<<8 ) / 16.00; // mT
  
  // Gyroscope
  gyrox = (int16_t)(Wire.read()|Wire.read()<<8 ) / 16.00; // Dps
  gyroy = (int16_t)(Wire.read()|Wire.read()<<8 ) / 16.00; // Dps
  gyroz = (int16_t)(Wire.read()|Wire.read()<<8 ) / 16.00; // Dps
  
  // Euler Angles
  Yaw=(int16_t)(Wire.read()|Wire.read()<<8 )/16.00;  // in Degrees unit
  Roll=(int16_t)(Wire.read()|Wire.read()<<8 )/16.00;  // in Degrees unit
  Pitch=(int16_t)(Wire.read()|Wire.read()<<8 )/16.00;  // in Degrees unit
  
  // Quaternions
  q0 = (int16_t)(Wire.read()|Wire.read()<<8 )/(pow(2,14)); // unit less
  q1 = (int16_t)(Wire.read()|Wire.read()<<8 )/(pow(2,14)); // unit less
  q2 = (int16_t)(Wire.read()|Wire.read()<<8 )/(pow(2,14)); // unit less
  q3 = (int16_t)(Wire.read()|Wire.read()<<8 )/(pow(2,14)); // unit less
  
  // Convert Quaternions to Euler Angles
  Yaw2 = (atan2(2*(q0*q3+q1*q2),1-2*(pow(q2,2)+pow(q3,2))))*180/PI;
  Roll2 = (asin(2*(q0*q2-q3*q1)))*180/PI;
  Pitch2 = (atan2(2*(q0*q1+q2*q3),1-2*(pow(q1,2)+pow(q2,2))))*180/PI;
  
  // Linear (Dynamic) & Gravitational (static) Acceleration
  Wire.beginTransmission(0x29);
  Wire.write(0x28);  
  Wire.endTransmission(false);
  Wire.requestFrom(0x29, 12, true);
  
  LIAx = (int16_t)(Wire.read()|Wire.read() << 8) / 100.00; // m/s^2
  LIAy = (int16_t)(Wire.read()|Wire.read() << 8) / 100.00; // m/s^2
  LIAz = (int16_t)(Wire.read()|Wire.read() << 8) / 100.00; // m/s^2
  GRVx = (int16_t)(Wire.read()|Wire.read() << 8) / 100.00; // m/s^2
  GRVy = (int16_t)(Wire.read()|Wire.read() << 8) / 100.00; // m/s^2
  GRVz = (int16_t)(Wire.read()|Wire.read() << 8) / 100.00; // m/s^2
  
  // Print data
//  if (q0 >= 0) {
//    Serial.print(" ");
//  }
//  Serial.print(q0);
//  Serial.print("; ");
//  
//  if (q1 >= 0) {
//    Serial.print(" ");
//  }
//  Serial.print(q1);
//  Serial.print("; ");
//  
//  if (q2 >= 0) {
//    Serial.print(" ");
//  }
//  Serial.print(q2);
//  Serial.print("; ");
//  
//  if (q2 >= 0) {
//    Serial.print(" ");
//  }
//  Serial.print(q2);
//  Serial.print(";  ");
  

  // Accelerometer values without gravity
  Serial.print(LIAx);
  Serial.print("; ");
  Serial.print(LIAy);
  Serial.print("; ");
  Serial.print(LIAz);
  Serial.println();

  char a[16];
  itoa(q0 * 100, a, 10);
  
  int dataToBeTransmitted[7] = {q1 * 100, q1 * 100, q2 * 100, q3 * 100, accx * 100, accy * 100, accz * 100};
//  int dataToBeTransmitted[5] = {'1', '2', '3', '4', '5'};
  
  radio.write(&dataToBeTransmitted, sizeof(dataToBeTransmitted));
  Serial.println(dataToBeTransmitted[0]);
  delay(1000);
}
