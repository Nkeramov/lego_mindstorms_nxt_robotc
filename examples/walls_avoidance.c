#pragma config(Sensor, S4, sonar, sensorSONAR)

const int STOP_DISTANCE = 40;       // cm
const int TURN_ENCODER_DEG = 245;   // ~90° for D=56 mm, track≈152 mm

// direction: +1 = right, -1 = left
void pivotTurn(int direction) {
    // stop first to avoid a current spike / mechanical shock
    motor[motorB] = 0;
    motor[motorC] = 0;
    wait1Msec(200);

    nMotorEncoder[motorB] = 0;
    // let the encoder reset
    wait1Msec(20);                     

    motor[motorB] = -60 * direction;
    motor[motorC] =  60 * direction;
    while (abs(nMotorEncoder[motorB]) < TURN_ENCODER_DEG) {
        wait1Msec(5);
    }

    motor[motorB] = 0;
    motor[motorC] = 0;
    wait1Msec(200);
}

task main() {
    while (true) {
        motor[motorB] = 60;
        motor[motorC] = 60;

        // move forward until a VALID reading shows an obstacle within range.
        // negative sonar values (no echo) and out-of-range values are treated
        // as "no obstacle" -> keep moving.
        d = SensorValue[sonar];
        while (d < 0 || d > STOP_DISTANCE) {
            wait1Msec(10);
            d = SensorValue[sonar];
        }

        // obstacle detected — choose a random direction
        int direction = (random(2) == 0) ? +1 : -1;
        pivotTurn(direction);
    }
}