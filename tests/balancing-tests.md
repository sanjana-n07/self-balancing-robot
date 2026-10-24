# Balancing Test Log

## 1. Project Information

| Parameter | Details |
|---|---|
| Project | Arduino Self-Balancing Robot |
| Controller | Arduino Uno |
| IMU sensor | MPU6050 |
| Motor driver | L298N |
| Control approach | Complementary filter and PID-based control |
| Main firmware | [`self_balancing_robot.ino`](../src/self_balancing_robot/self_balancing_robot.ino) |
| Sensor test | [`mpu6050-test.ino`](mpu6050-test.ino) |
| Motor test | [`motor-test.ino`](motor-test.ino) |

## 2. Testing Objectives

The objective is to verify sensor communication, motor operation, angle estimation, and closed-loop balancing behaviour.

Testing is divided into individual hardware checks and full-system balancing tests so that faults can be isolated systematically.

## 3. Component-Level Tests

### 3.1 MPU6050 Sensor Test

**Test procedure**

1. Upload the standalone MPU6050 test sketch.
2. Open the Serial Monitor at the baud rate specified in the sketch.
3. Verify that the sensor responds at its configured I²C address.
4. Observe accelerometer readings while the robot is stationary.
5. Tilt the robot forward and backward and observe the readings.
6. Rotate the robot slowly and check the gyroscope response.

**Status:** Record the result after running the standalone test.

**Observations**

- Sensor detection:
- Accelerometer response:
- Gyroscope response:
- Communication errors:

### 3.2 Motor Driver Test

**Test procedure**

1. Secure the robot with its wheels clear of the ground.
2. Verify the motor supply and common ground.
3. Upload the standalone motor test sketch.
4. Test forward, backward, and stop commands.
5. Check whether speed commands change the motor speed.
6. Verify that each motor's direction matches the intended command.

**Status:** Record the result after running the standalone test.

**Observations**

- Forward operation:
- Backward operation:
- Stop operation:
- PWM response:
- Left/right motor differences:

## 4. Full-System Balancing Tests

| Test | Expected behaviour | Status |
|---|---|---|
| Sensor angle estimation | Tilt estimate changes consistently with robot orientation | Pending verification |
| Forward-tilt response | Wheels move forward to catch the falling body | Pending verification |
| Backward-tilt response | Wheels move backward to catch the falling body | Pending verification |
| Motor correction | Motor response changes according to the control output | Pending verification |
| Brief balancing | Robot remains near upright for a measurable interval | In progress |
| Sustained balancing | Robot balances reliably without external support | Not yet verified |

The robot has demonstrated partial operation, but reliable sustained balancing has not yet been established.

## 5. Known Development Areas

The following areas require further testing and optimisation:

- PID gain tuning and integral windup prevention
- Verification of sensor-axis orientation and motor direction
- Motor deadband and minimum effective PWM calibration
- Independent control of both motors
- Sensor communication failure handling
- Power-supply stability under motor load
- Control-loop timing and measurement consistency

These are development areas to investigate, not confirmed faults in every case.

## 6. Test Session Record

Copy this section for each new test session.

### Session

- **Date:**
- **Firmware version or commit:**
- **Changes made:**
- **Test conditions:**
- **Observed behaviour:**
- **Result:** Pass / Fail / Inconclusive
- **Next action:**

## 7. Safety

- Keep the robot restrained or supported during initial tests.
- Keep wheels clear of the ground during motor direction testing.
- Disconnect motor power before changing wiring.
- Stop testing if the robot moves unexpectedly, the driver overheats, or the supply becomes unstable.

## 8. Future Improvements

- Record the maximum unsupported balancing duration.
- Measure tilt variation during stable operation.
- Record motor response and sensor data for repeatable comparisons.
- Compare controller settings across test sessions.
- Add test videos and plots to document improvements.

---

*Test status should be updated only after the corresponding test has been performed.*
