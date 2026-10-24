
/*
  L298N Motor Test
  Arduino Uno
  ENA = 9 (PWM)
  IN1 = 2
  IN2 = 3

  Serial commands:
    F = Forward
    B = Backward
    S = Stop
    + = Increase speed
    - = Decrease speed
*/

const uint8_t ENA_PIN = 9;
const uint8_t IN1_PIN = 2;
const uint8_t IN2_PIN = 3;

int motorSpeed = 100;
const int MIN_TEST_PWM = 70;
const int MAX_TEST_PWM = 180;

void stopMotor() {
  analogWrite(ENA_PIN, 0);
  digitalWrite(IN1_PIN, LOW);
  digitalWrite(IN2_PIN, LOW);
}

void setMotor(bool forward, int pwm) {
  // Stop PWM before changing direction.
  analogWrite(ENA_PIN, 0);

  if (forward) {
    digitalWrite(IN1_PIN, HIGH);
    digitalWrite(IN2_PIN, LOW);
  } else {
    digitalWrite(IN1_PIN, LOW);
    digitalWrite(IN2_PIN, HIGH);
  }

  analogWrite(ENA_PIN, constrain(pwm, 0, 255));
}

void setup() {
  pinMode(ENA_PIN, OUTPUT);
  pinMode(IN1_PIN, OUTPUT);
  pinMode(IN2_PIN, OUTPUT);

  stopMotor();

  Serial.begin(115200);

  Serial.println(F("\nL298N Motor Test"));
  Serial.println(F("F = Forward"));
  Serial.println(F("B = Backward"));
  Serial.println(F("S = Stop"));
  Serial.println(F("+ = Increase speed"));
  Serial.println(F("- = Decrease speed"));
  Serial.println(F("Motor starts STOPPED."));
  Serial.println(F("Keep wheels lifted and clear."));
}

void loop() {
  if (!Serial.available()) return;

  char command = Serial.read();

  switch (command) {
    case 'F':
    case 'f':
      setMotor(true, motorSpeed);
      Serial.print(F("Forward, PWM = "));
      Serial.println(motorSpeed);
      break;

    case 'B':
    case 'b':
      setMotor(false, motorSpeed);
      Serial.print(F("Backward, PWM = "));
      Serial.println(motorSpeed);
      break;

    case 'S':
    case 's':
      stopMotor();
      Serial.println(F("Motor STOPPED"));
      break;

    case '+':
      motorSpeed = min(motorSpeed + 10, MAX_TEST_PWM);
      Serial.print(F("Speed setting = "));
      Serial.println(motorSpeed);
      break;

    case '-':
      motorSpeed = max(motorSpeed - 10, MIN_TEST_PWM);
      Serial.print(F("Speed setting = "));
      Serial.println(motorSpeed);
      break;

    case '\r':
    case '\n':
      break;

    default:
      Serial.println(F("Unknown command. Use F/B/S/+/-"));
      break;
  }
}
