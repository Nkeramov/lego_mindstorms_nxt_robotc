task main() {
    eraseDisplay();

    const int D = 56;               // wheel diameter, mm
    const int L = 100;              // travel distance, cm
    const int POWER = 50;           // motor power

    // compute path using the original formula
    // rotations = (L * 10) / (PI * D)  — number of wheel rotations
    // degrees  = rotations * 360       — encoder degrees
    // L*10 converts cm to mm; PI*D is the wheel circumference in mm
    float pathF = ((float)L * 10.0) / (PI * D) * 360.0;
    // round to the nearest whole degree
    int path  = (int)(pathF + 0.5);        

    // motor synchronization
    // B is the master, C is the slave, ratio 1:1
    nSyncedMotors    = synchBC;
    nSyncedTurnRatio = 100;
    // reset the master motor encoder
    nMotorEncoder[motorB] = 0;
    // let the encoder reset
    wait1Msec(50);                            

    // set the target and start
    // encoder target in degrees (for the master motor)
    nMotorEncoderTarget[motorB] = path;
    // motorC follows automatically
    motor[motorB] = POWER;                    

    // wait until the master motor stops
    while (nMotorRunState[motorB] != runStateIdle) {
        displayBigTextLine(0, "p =%d", path);
        displayBigTextLine(2, "B =%d", nMotorEncoder[motorB]);
        displayBigTextLine(4, "C =%d", nMotorEncoder[motorC]);
        wait1Msec(20);
    }
    
    // explicit stop
    motor[motorB] = 0;
    motor[motorC] = 0;
    playSound(soundBeepBeep);
}