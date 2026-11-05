#pragma config(Sensor, S1,     ,               sensorLightActive)
#pragma config(Sensor, S2,     ,               sensorLightActive)
#pragma config(Sensor, S3,     ,               sensorTouch)


// PID coefficients
float kp = 1.65;
float ki = 0.012;
float kd = 0.01;

// motor power limits
int m_min = 12, m_max = 90;
// starting power for both motors
int BASE  = 50;

int clampPower(int val, int min, int max) {
	return (val < min) ? min : ((val > max) ? max : val);
}

// calibration data (filled during the calibration phase)
int l_white = 50, l_black = 10;
int r_white = 50, r_black = 10;

// normalize a raw light reading to 0..100 for its sensor
int norm(int raw, int white, int black) {
    int range = white - black;
    // avoid division by zero
    if (range == 0) return 50;
    int v = (raw - black) * 100 / range;
    return clampPower(v, 0, 100);
}

// wait for the touch sensor to be pressed, then released
void waitForTouch() {
    while(SensorValue[S3] == 0) wait1Msec(10);
    while(SensorValue[S3] == 1) wait1Msec(10);
}

task main() {
    eraseDisplay();

    // calibration: WHITE surface
    displayCenteredBigTextLine(2, "Put on WHITE");
    displayCenteredBigTextLine(4, "Press S3");
    waitForTouch();
    wait1Msec(300);
    l_white = SensorValue[S1];
    r_white = SensorValue[S2];

    // calibration: BLACK line
    eraseDisplay();
    displayCenteredBigTextLine(2, "Put on BLACK");
    displayCenteredBigTextLine(4, "Press S3");
    waitForTouch();
    wait1Msec(300);
    l_black = SensorValue[S1];
    r_black = SensorValue[S2];

    // show calibration
    eraseDisplay();
    displayBigTextLine(0, "W %d %d", l_white, r_white);
    displayBigTextLine(2, "B %d %d", l_black, r_black);
    displayBigTextLine(6, "Press S3");
    waitForTouch();

    // ready
    eraseDisplay();
    displayCenteredBigTextLine(2, "LINE TRACE");
    displayCenteredBigTextLine(4, "Press S3");
    waitForTouch();
    playSound(soundBeepBeep);

    // initial state
    int d1 = norm(SensorValue[S1], l_white, l_black);
    int d2 = norm(SensorValue[S2], r_white, r_black);

    float m1 = BASE, m2 = BASE;
    float e_cur   = d1 - d2;
    float e_prev  = e_cur;
    float e_prev2 = e_cur;

    motor[motorB] = (int)m1;
    motor[motorC] = (int)m2;

    // main control loop
    while(true) {
        d1 = norm(SensorValue[S1], l_white, l_black);
        d2 = norm(SensorValue[S2], r_white, r_black);
        e_cur = d1 - d2;

        // incremental (velocity-form) PID:
        // delta = Kp*(e - e_prev) + Ki*e + Kd*(e - 2*e_prev + e_prev2)
        float delta = kp * (e_cur - e_prev) + ki * e_cur + kd * (e_cur - 2*e_prev + e_prev2);

        m1 = clampPower(m1 + delta, m_min, m_max);
        m2 = clampPower(m2 - delta, m_min, m_max);

        motor[motorB] = (int)m1;
        motor[motorC] = (int)m2;

        // debug display
        displayBigTextLine(0, "e=%d", (int)e_cur);
        displayBigTextLine(2, "L=%d", d1);
        displayBigTextLine(4, "R=%d", d2);
        displayBigTextLine(6, "B=%d C=%d", (int)m1, (int)m2);

        e_prev2 = e_prev;
        e_prev  = e_cur;

        // sets the sample period T
        wait1Msec(10);
    }
}
