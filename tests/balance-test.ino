
#include <Wire.h>
#include <math.h>

// ---------- MPU6050 ----------
const uint8_t MPU_ADDR = 0x68;

// ---------- L298N: one motor channel ----------
const uint8_t ENA_PIN = 9;
const uint8_t IN1_PIN = 2;
const uint8_t IN2_PIN = 3;

// ---------- Controller settings ----------
float Kp = 5.0;
float Kd = 0.6;

const float ALPHA = 0.98;
const float FALL_ANGLE_DEG = 25.0;
const int MIN_PWM = 55;
const int MAX_PWM = 180;

float baseTilt = 0.0;
float currentAngle = 0.0;

unsigned long lastLoopUs = 0;
unsigned long lastPrintMs = 0;

bool enabled = false;

// ---------- MPU6050 helpers ----------
int16_t read16() {
  uint8_t hi = Wire.read();
  uint8_t lo = Wire.read();
  return (int16_t)(((uint16_t)hi << 8) | lo);
}

bool readIMU(int16_t &ax, int16_t &az, int16_t &gy) {
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x3B);

  if (Wire.endTransmission(false) != 0) {
    return false;
  }

  if (Wire.requestFrom(MPU_ADDR, (uint8_t)14,
                       (uint8_t)true) != 14) {
    return false;
  }

  ax = read16();
  read16();        // ay
  az = read16();
  read16();        // temperature
  read16();        // gx
  gy = read16();
  read16();        // gz

  return true;
}

float accelAngle(int16_t ax, int16_t az) {
  return atan2((float)ax, (float)az) * RAD_TO_DEG;
}

// ---------- Motor control ----------
void motorStop() {
  analogWrite(ENA_PIN, 0);
  digitalWrite(IN1_PIN, LOW);
  digitalWrite(IN2_PIN, LOW);
}

void driveMotor(float output) {
  if (output > 0) {
    digitalWrite(IN1_PIN, HIGH);
    digitalWrite(IN2_PIN, LOW);
  } else {
    digitalWrite(IN1_PIN, LOW);
    digitalWrite(IN2_PIN, HIGH);
  }

  int pwm = MIN_PWM + (int)fabs(output);
  pwm = constrain(pwm, MIN_PWM, MAX_PWM);

  analogWrite(ENA_PIN, pwm);
}

// ---------- Setup ----------
void setup() {
  pinMode(ENA_PIN, OUTPUT);
  pinMode(IN1_PIN, OUTPUT);
  pinMode(IN2_PIN, OUTPUT);

  motorStop();

  Serial.begin(115200);
  Wire.begin();

  #if defined(WIRE_HAS_TIMEOUT)
    Wire.setWireTimeout(3000, true);
  #endif

  // Wake MPU6050.
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x6B);
  Wire.write(0x00);

  if (Wire.endTransmission() != 0) {
    Serial.println(F("MPU6050 initialization failed."));
    while (true) {
      motorStop();
      delay(100);
    }
  }

  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x1A);
  Wire.write(0x04);
  Wire.endTransmission();

  Serial.println(F("\nBalance Test"));
  Serial.println(F("Keep robot supported during calibration."));
  Serial.println(F("Commands: E=enable, S=stop, R=calibrate"));
  Serial.println(F("Angle, gyroRate, output, enabled"));

  // Calibrate with robot held at the desired upright angle.
  float sum = 0.0;
  const int samples = 200;

  for (int i = 0; i < samples; i++) {
    int16_t ax, az, gy;

    if (!readIMU(ax, az, gy)) {
      Serial.println(F("Sensor read failed during calibration."));
      while (true) {
        motorStop();
        delay(100);
      }
    }

    sum += accelAngle(ax, az);
    delay(5);
  }

  baseTilt = sum / samples;
  currentAngle = 0.0;

  lastLoopUs = micros();

  Serial.print(F("Upright reference: "));
  Serial.println(baseTilt, 2);
  Serial.println(F("Motor output remains disabled. Send E to enable."));
}

// ---------- Main loop ----------
void loop() {
  // Serial safety commands.
  while (Serial.available()) {
    char c = Serial.read();

    if (c == 'S' || c == 's') {
      enabled = false;
      motorStop();
      Serial.println(F("MOTOR STOPPED"));
    }

    if (c == 'E' || c == 'e') {
      enabled = true;
      lastLoopUs = micros();
      Serial.println(F("CONTROL ENABLED"));
    }

    if (c == 'R' || c == 'r') {
      enabled = false;
      motorStop();
      Serial.println(F("Restart board to recalibrate safely."));
    }
  }

  unsigned long nowUs = micros();
  float dt = (nowUs - lastLoopUs) * 1e-6;

  if (dt < 0.01) return;

  lastLoopUs = nowUs;

  // Reject unusually long intervals.
  if (dt > 0.05) {
    motorStop();
    enabled = false;
    Serial.println(F("Loop timing fault: motor disabled."));
    return;
  }

  int16_t ax, az, gy;

  if (!readIMU(ax, az, gy)) {
    motorStop();
    enabled = false;
    Serial.println(F("IMU read failure: motor disabled."));
    return;
  }

  float accError = accelAngle(ax, az) - baseTilt;
  float gyroRate = gy / 131.0;

  currentAngle =
      ALPHA * (currentAngle + gyroRate * dt)
      + (1.0 - ALPHA) * accError;

  float output = Kp * currentAngle + Kd * gyroRate;

  if (fabs(currentAngle) > FALL_ANGLE_DEG) {
    motorStop();
    enabled = false;
    Serial.println(F("FALL LIMIT: motor disabled."));
    return;
  }

  if (enabled) {
    driveMotor(output);
  } else {
    motorStop();
  }

  if (millis() - lastPrintMs >= 100) {
    lastPrintMs = millis();

    Serial.print(currentAngle, 2);
    Serial.print('\t');
    Serial.print(gyroRate, 2);
    Serial.print('\t');
    Serial.print(output, 2);
    Serial.print('\t');
    Serial.println(enabled ? F("ON") : F("OFF"));
  }
}
