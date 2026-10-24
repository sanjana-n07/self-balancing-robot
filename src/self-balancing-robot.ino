#include <Wire.h>
#include <math.h>

// ---------- Hardware Pins ----------
const uint8_t ENA_PIN = 9;   // PWM speed pin
const uint8_t IN1_PIN = 2;   // Direction pin 1
const uint8_t IN2_PIN = 3;   // Direction pin 2 (IN1 HIGH / IN2 LOW = FORWARD)

#define MPU_ADDR 0x68

// ---------- Tuned PID Constants ----------
float Kp = 5.0;              // Your working proportional gain
float Ki = 100;              // Keep at 0.0 (only add ~0.3 if it slowly drifts one way)
float Kd = 0.6;             // Increased from 0.6 to 0.85 to dampen the minor wobble

const int MIN_PWM = 55;              // Lowered from 64 to 55 for smoother zero-crossing
const float DEADZONE_DEG = 0.5;      // Tightened from 1.0 to 0.5 deg to prevent hunting
const float FALL_ANGLE_DEG = 45.0;   // Safety cutoff if tilted past 45 degrees
const float COMP_ALPHA = 0.98;       // 98% gyro, 2% accelerometer blend

// ---------- State Variables ----------
const float GYRO_SCALE = 131.0;
float baseTilt = 0.0;
float gyroBiasY = 0.0;
float currentAngle = 0.0;
float iTerm = 0.0;

unsigned long lastLoopUs = 0;
unsigned long lastPrintMs = 0;

// ---------- Helper Functions ----------
bool initMPU() {
  // 1. Wake up the MPU
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x6B);
  Wire.write(0x00);
  if (Wire.endTransmission() != 0) return false;

  // 2. Enable built-in 20 Hz Digital Low-Pass Filter (DLPF) in register 0x1A
  // 0x04 = 20 Hz (reduces phase lag compared to 5 Hz to stop wobbling)
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x1A);
  Wire.write(0x04);
  return (Wire.endTransmission() == 0);
}

int16_t rd16() {
  uint8_t hi = Wire.read();
  uint8_t lo = Wire.read();
  return (int16_t)((hi << 8) | lo);
}

bool readIMU(int16_t &ax, int16_t &az, int16_t &gy) {
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x3B);
  if (Wire.endTransmission(false) != 0) return false;
  if (Wire.requestFrom((uint8_t)MPU_ADDR, (uint8_t)14, (uint8_t)true) != 14) return false;
  ax = rd16();
  rd16(); // skip ay
  az = rd16();
  rd16(); // skip temp
  rd16(); // skip gx
  gy = rd16();
  rd16(); // skip gz
  return true;
}

float tiltDeg(int16_t ax, int16_t az) {
  return atan2((float)ax, (float)az) * RAD_TO_DEG;
}

void motorStop() {
  analogWrite(ENA_PIN, 0);
  digitalWrite(IN1_PIN, LOW);
  digitalWrite(IN2_PIN, LOW);
}

int applyMotors(float error, float pidOut) {
  // 1. Safety shutoff if the robot falls over
  if (fabs(error) > FALL_ANGLE_DEG) {
    motorStop();
    iTerm = 0;
    return 0;
  }

  // 2. Small neutral zone right at the balance point
  if (fabs(error) < DEADZONE_DEG) {
    motorStop();
    return 0;
  }

  // 3. Add minimum PWM offset to overcome gearbox friction
  int pwm = MIN_PWM + (int)fabs(pidOut);
  if (pwm > 255) pwm = 255;

  // 4. Set motor direction (Positive PID = Forward)
  if (pidOut > 0) {
    digitalWrite(IN1_PIN, HIGH);
    digitalWrite(IN2_PIN, LOW);
  } else {
    digitalWrite(IN1_PIN, LOW);
    digitalWrite(IN2_PIN, HIGH);
  }

  analogWrite(ENA_PIN, pwm);
  return (pidOut > 0) ? pwm : -pwm;
}

// ---------- Setup ----------
void setup() {
  Serial.begin(115200);
  pinMode(ENA_PIN, OUTPUT);
  pinMode(IN1_PIN, OUTPUT);
  pinMode(IN2_PIN, OUTPUT);
  motorStop();

  Wire.begin();
  Wire.setWireTimeout(3000, true); // Auto-reset I2C bus on electrical noise spikes

  Serial.println(F("Initializing MPU with 20 Hz hardware filter..."));
  while (!initMPU()) {
    Serial.println(F("MPU not responding, retrying in 1 second..."));
    delay(1000);
  }

  Serial.println(F("Hold robot upright and steady at balance point (calibrating for 3 s)..."));
  delay(3000);

  float aSum = 0, gSum = 0;
  int n = 0;
  for (int i = 0; i < 200; i++) {
    int16_t ax, az, gy;
    if (readIMU(ax, az, gy)) {
      aSum += tiltDeg(ax, az);
      // Negative sign aligns Y-gyro direction with +X forward tilt
      gSum += -(gy / GYRO_SCALE);
      n++;
    }
    delay(5);
  }

  if (n < 50) {
    Serial.println(F("Calibration failed! Check I2C wiring."));
    while (1) {}
  }

  baseTilt = aSum / n;
  gyroBiasY = gSum / n;
  currentAngle = 0.0;

  Serial.print(F("Calibrated! Base upright angle: "));
  Serial.println(baseTilt, 1);
  Serial.println(F("System Armed and Balancing!"));
  lastLoopUs = micros();
}

// ---------- Main Loop (100 Hz) ----------
void loop() {
  unsigned long nowUs = micros();
  if (nowUs - lastLoopUs < 10000) return; // Strict 10 ms (100 Hz) loop
  float dt = (nowUs - lastLoopUs) * 1e-6;
  lastLoopUs = nowUs;

  int16_t ax, az, gy;
  if (!readIMU(ax, az, gy)) {
    initMPU(); // Re-initialize if an I2C packet drops
    return;
  }

  // 1. Accelerometer angle relative to calibrated balance point
  float accError = tiltDeg(ax, az) - baseTilt;

  // 2. Gyro angular rate (deg/s) aligned with forward tilt
  float gyroRate = -(gy / GYRO_SCALE) - gyroBiasY;

  // 3. Complementary filter combines fast gyro with drift-free accelerometer
  currentAngle = COMP_ALPHA * (currentAngle + gyroRate * dt) + (1.0 - COMP_ALPHA) * accError;

  // 4. PID Calculation
  float error = currentAngle;
  iTerm += (error * dt);
  iTerm = constrain(iTerm, -50.0, 50.0);

  float pTerm = Kp * error;
  float dTerm = Kd * gyroRate;
  float pidOut = pTerm + (Ki * iTerm) + dTerm;

  int appliedPwm = applyMotors(error, pidOut);

  // 5. English Serial Telemetry every 200 ms
  if (millis() - lastPrintMs >= 200) {
    lastPrintMs = millis();
    Serial.print(F("Tilt: ")); Serial.print(error, 1);
    Serial.print(F(" deg | Gyro: ")); Serial.print(gyroRate, 1);
    Serial.print(F(" deg/s | P: ")); Serial.print(pTerm, 1);
    Serial.print(F(" | D: ")); Serial.print(dTerm, 1);
    Serial.print(F(" | PWM: ")); Serial.println(appliedPwm);
  }
}
