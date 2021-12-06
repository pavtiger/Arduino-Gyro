// Connect GND, S1 and SR pins together.

#include <Wire.h>

// Radio
#include <nRF24L01.h>
#include <RF24.h>

RF24 radio(9, 8);  // Create radio object with RF24 library

float Yaw, Roll, Pitch, magx, magy, magz, accx, accy, accz, gyrox, gyroy, gyroz, q0, q1, q2, q3, Roll2, Pitch2, Yaw2, LIAx, LIAy, LIAz, GRVx, GRVy, GRVz;
const int GY_955 = 0x29;


struct Quaternion {
    double w, x, y, z;
};

struct EulerAngle {
    double roll, pitch, yaw;
};


EulerAngle ToEulerAngles(Quaternion q) {
    EulerAngle angles;

    // roll (x-axis rotation)
    double sinr_cosp = 2 * (q.w * q.x + q.y * q.z);
    double cosr_cosp = 1 - 2 * (q.x * q.x + q.y * q.y);
    angles.roll = atan2(sinr_cosp, cosr_cosp);

    // pitch (y-axis rotation)
    double sinp = 2 * (q.w * q.y - q.z * q.x);
    if (abs(sinp) >= 1)
        angles.pitch = copysign(M_PI / 2, sinp); // use 90 degrees if out of range
    else
        angles.pitch = asin(sinp);

    // yaw (z-axis rotation)
    double siny_cosp = 2 * (q.w * q.z + q.x * q.y);
    double cosy_cosp = 1 - 2 * (q.y * q.y + q.z * q.z);
    angles.yaw = atan2(siny_cosp, cosy_cosp);

    return angles;
}


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
  q0 = (int16_t)(Wire.read()|Wire.read()<<8 ) / (pow(2,14)); // unit less
  q1 = (int16_t)(Wire.read()|Wire.read()<<8 ) / (pow(2,14)); // unit less
  q2 = (int16_t)(Wire.read()|Wire.read()<<8 ) / (pow(2,14)); // unit less
  q3 = (int16_t)(Wire.read()|Wire.read()<<8 ) / (pow(2,14)); // unit less
  
  // Convert Quaternions to Euler Angles
  Yaw2 = (atan2(2*(q0*q3+q1*q2),1-2 * (pow(q2,2) + pow(q3,2)))) * 180 / PI;
  Roll2 = (asin(2*(q0*q2-q3*q1)))*180/PI;
  Pitch2 = (atan2(2*(q0*q1+q2*q3),1-2 * (pow(q1,2) + pow(q2,2)))) * 180 / PI;
  
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


  // convert to Euler manually
  Quaternion q;
  q.x = q0;
  q.y = q1;
  q.z = q2;
  q.w = q3;
  EulerAngle converted = ToEulerAngles(q);
  Serial.print(converted.roll * 180 / PI);
  Serial.print("; ");
  Serial.print(converted.pitch * 180 / PI);
  Serial.print("; ");
  Serial.print(converted.yaw * 180 / PI);
  Serial.println();
  
  
//  int dataToBeTransmitted[6] = {converted.roll * 1000, converted.pitch * 1000, converted.yaw * 1000, LIAx * 1000, LIAy * 1000, LIAz * 1000};
  int dataToBeTransmitted[7] = {q0 * 1000, q1 * 1000, q2 * 1000, q3 * 1000, LIAx * 1000, LIAy * 1000, LIAz * 1000};
  
  radio.write(&dataToBeTransmitted, sizeof(dataToBeTransmitted));
  Serial.println("sent data");
  delay(10);
}
