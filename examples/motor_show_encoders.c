#pragma config(Sensor, S1, conveyor, sensorTouch)

task main(){
    const int kPollInterval = 20;   // ms between polls

    // initial reset
    nMotorEncoder[motorA] = 0;
    nMotorEncoder[motorB] = 0;
    nMotorEncoder[motorC] = 0;

    int  lastA     = -1;
    int  lastB     = -1;
    int  lastC     = -1;
    int  lastTouch = -1;
    int  lastBtn   = kNoButton;
    bool running   = true;

    eraseDisplay();
    displayBigTextLine(0, "Encoder view");

    while (running){
        // read current state
        int a = nMotorEncoder[motorA];
        int b = nMotorEncoder[motorB];
        int c = nMotorEncoder[motorC];
        int t = SensorValue[conveyor];

        // refresh display only if something changed
        if (a != lastA || b != lastB || c != lastC || t != lastTouch){
            displayCenteredTextLine(1, "Touch: %d", t);
            displayBigTextLine(3, "A: %d", a);
            displayBigTextLine(5, "B: %d", b);
            displayBigTextLine(7, "C: %d", c);
            lastA = a;
            lastB = b;
            lastC = c;
            lastTouch = t;
        }

        // buttons: Enter = reset, Escape = quit
        int btn = nNxtButtonPressed;
        if (btn != lastBtn){
            if (btn == kEnterButton){
                nMotorEncoder[motorA] = 0;
                nMotorEncoder[motorB] = 0;
                nMotorEncoder[motorC] = 0;
                // force redraw on next iteration
                lastA = lastB = lastC = -1;
            }
            else if (btn == kExitButton){
                running = false;
            }
            lastBtn = btn;
        }
        wait1Msec(kPollInterval);
    }

    // shutdown
    eraseDisplay();
    displayCenteredBigTextLine(3, "Stopped");
    playSound(soundBlip);
    while (bSoundActive);
}