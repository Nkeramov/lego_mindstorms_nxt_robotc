task main() {
    eraseDisplay();

    const int D = 56;               // wheel diameter, mm
    const int L = 100;              // travel distance, cm
    const int BASE_POWER = 50;      // base motor power
    const int MIN_POWER  = 10;      // below this motors may stall
    const float kp = 2.0;           // proportional gain

    // compute path using the original formula
    // rotations = (L * 10) / (PI * D)  — number of wheel rotations
    // degrees  = rotations * 360       — encoder degrees
    // L*10 converts cm to mm; PI*D is the wheel circumference in mm
    float pathF = ((float)L * 10.0) / (PI * D) * 360.0;
    // round to the nearest whole degree
    int path  = (int)(pathF + 0.5);     

    // reset encoders
    nMotorEncoder[motorB] = 0;
    nMotorEncoder[motorC] = 0;
    wait1Msec(50);

    // start at base power
    motor[motorB] = BASE_POWER;
    motor[motorC] = BASE_POWER;

    // main control loop
    while (true) {
        int encB = abs(nMotorEncoder[motorB]);
        int encC = abs(nMotorEncoder[motorC]);

        // stop both motors together when both wheels have covered the path
        if (encB >= path && encC >= path) break;

        // error: how much C is ahead of B (positive → C is ahead)
        int err = encC - encB;

        // proportional correction
        // if C is ahead (err > 0), slow C down and speed B up.
        int u = (int)(err * kp);

        int mB = constrain(BASE_POWER + u, MIN_POWER, 100);
        int mC = constrain(BASE_POWER - u, MIN_POWER, 100);

        // if one wheel has already reached the target — freeze it
        // (prevents it from overshooting while the other still catches up)
        if (encB >= path) mB = 0;
        if (encC >= path) mC = 0;

        motor[motorB] = mB;
        motor[motorC] = mC;

        // debug display
        displayBigTextLine(0, "e =%d", err);
        displayBigTextLine(2, "u =%d", u);
        displayBigTextLine(4, "B =%d", encB);
        displayBigTextLine(6, "C =%d", encC);
        displayBigTextLine(8, "p =%d", path);

        wait1Msec(10);
    }

    motor[motorB] = 0;
    motor[motorC] = 0;
    playSound(soundBeepBeep);
}