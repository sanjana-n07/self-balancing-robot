
# Balancing Test Results

## Test Environment

- Controller: Arduino Uno
- IMU: MPU6050
- Motor driver: L298N
- Main sketch: `src/self_balancing_robot/self_balancing_robot.ino`

## MPU6050 Test

- [ ] Sensor detected at address 0x68
- [ ] Accelerometer responds to tilt
- [ ] Gyroscope responds to rotation
- [ ] Readings remain stable when stationary

Observations:

_Add actual results here._

## Motor Test

- [ ] Forward command works
- [ ] Backward command works
- [ ] Stop command works
- [ ] Speed changes affect the motor

Observations:

_Record motor direction, starting PWM and any issues._

## Balancing Test

- [ ] Robot responds to forward tilt
- [ ] Robot responds to backward tilt
- [ ] Robot can remain upright briefly
- [ ] Robot balances reliably

Observations:

_Record the actual behaviour. Do not mark tests complete until verified._

## Known Issues

- Record any sensor communication failures.
- Record any motor direction or power issues.
- Record any oscillation or instability.

## Future Improvements

- Tune PID gains systematically.
- Add independent control for both motors.
- Improve sensor failure handling.
- Measure loop timing and balancing performance.
