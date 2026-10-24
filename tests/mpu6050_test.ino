
#include <Wire.h>
#include <math.h>

const uint8_t MPU_ADDR = 0x68;
const float GYRO_SCALE = 131.0;  // Default +/-250 deg/s range

int16_t readInt16() {
  uint8_t hi = Wire.read();
  uint8_t lo = Wire.read();
  return (int16_t)(((uint16_t)hi << 8) | lo);
}

bool readMPU(int16_t &ax, int16_t &ay, int16_t &az,
             int16_t &gx, int16_t &gy, int16_t &gz) {
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x3B);

  if (Wire.endTransmission(false) != 0) return false;

  if (Wire.requestFrom(MPU_ADDR, (uint8_t)14, (uint8_t)true) != 14) {
    return false;
  }

  ax = readInt16();
  ay = readInt16();
  az = readInt16();
  readInt16();  // Temperature
  gx = readInt16();
  gy = readInt16();
  gz = readInt16();

  return true;
}

void setup() {
  Serial.begin(115200);
  Wire.begin();

  // Prevent an I2C bus hang on supported Arduino cores.
  #if defined(WIRE_HAS_TIMEOUT)
    Wire.setWireTimeout(3000, true);
  #endif

  Serial.println(F("\nMPU6050 Sensor Test"));

  // Check WHO_AM_I register (normally returns 0x68).
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x75);

  if (Wire.endTransmission(false) != 0 ||
      Wire.requestFrom(MPU_ADDR, (uint8_t)1, (uint8_t)true) != 1) {
    Serial.println(F("ERROR: MPU6050 not detected at 0x68."));
    Serial.println(F("Check VCC, GND, SDA and SCL."));
    while (true) delay(100);
  }

  uint8_t identity = Wire.read();

  Serial.print(F("WHO_AM_I = 0x"));
  Serial.println(identity, HEX);

  if (identity != 0x68) {
    Serial.println(F("Unexpected identity. Check the sensor/module."));
  }

  // Wake the sensor.
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x6B);
  Wire.write(0x00);

  if (Wire.endTransmission() != 0) {
    Serial.println(F("ERROR: Could not wake MPU6050."));
    while (true) delay(100);
  }

  // Configure the digital low-pass filter.
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x1A);
  Wire.write(0x04);
  Wire.endTransmission();

  delay(100);

  Serial.println(F("Reading sensor. Keep robot still, then tilt it."));
  Serial.println(F("Accel X Y Z | Gyro X Y Z | Angle XZ"));
}

void loop() {
  int16_t ax, ay, az, gx, gy, gz;

  if (!readMPU(ax, ay, az, gx, gy, gz)) {
    Serial.println(F("ERROR: Failed to read sensor."));
    delay(500);
    return;
  }

  float angle = atan2((float)ax, (float)az) * RAD_TO_DEG;

  Serial.print(ax); Serial.print('\t');
  Serial.print(ay); Serial.print('\t');
  Serial.print(az); Serial.print('\t');

  Serial.print(gx); Serial.print('\t');
  Serial.print(gy); Serial.print('\t');
  Serial.print(gz); Serial.print('\t');

  Serial.println(angle, 2);

  delay(100);
}
