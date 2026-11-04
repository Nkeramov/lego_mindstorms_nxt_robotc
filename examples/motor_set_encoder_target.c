task main(){
    const int kTargetCounts   = 720;   // encoder counts to move
    const int kPower          = 25;    // motor power (0..100)
    const int kStartupTimeout = 500;   // ms to wait for motor to start
    const int kRunTimeout     = 5000;  // ms max run time
    const int kPollInterval   = 10;    // ms between state polls

    // reset encoder and set target relative to current position
    motor[motorA] = 0;
    nMotorEncoder[motorA] = 0;
    int startPos = nMotorEncoder[motorA];
    nMotorEncoderTarget[motorA] = startPos + kTargetCounts;

    // show initial info
    eraseDisplay();
    displayBigTextLine(0, "Enc target test");
    displayBigTextLine(2, "Target: %d", kTargetCounts);

    // start motor
    motor[motorA] = kPower;

    // wait for the motor to leave the idle state.
    // if the target is already reached, the motor may stay idle -- this is
    // handled below via the 'started' flag.
    int startupTimeout = kStartupTimeout;
    while (nMotorRunState[motorA] == runStateIdle && startupTimeout > 0){
        wait1Msec(kPollInterval);
        startupTimeout -= kPollInterval;
    }
    int started = (nMotorRunState[motorA] != runStateIdle);

    // wait for the motor to finish (or time out)
    int runTimeout = kRunTimeout;
    while (nMotorRunState[motorA] != runStateIdle && runTimeout > 0){
        wait1Msec(kPollInterval);
        runTimeout -= kPollInterval;
    }
    int finished = (nMotorRunState[motorA] == runStateIdle);

    // stop motor explicitly
    motor[motorA] = 0;

    // report result
    displayBigTextLine(4, "Pos: %d", nMotorEncoder[motorA]);

    if (!started && finished)
        displayBigTextLine(6, "No motion");
    else if (!finished)
        displayBigTextLine(6, "TIMEOUT");
    else {
        displayBigTextLine(6, "Done");
		displayBigTextLine(7, "Err: %d", nMotorEncoder[motorA] - (startPos + kTargetCounts));
	}
    playSound(soundBlip);
    while (bSoundActive);
}