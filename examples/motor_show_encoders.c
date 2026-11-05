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
    displayBigTextLine(0, "Encoders");

    while (running){
        // read current state
        int a = nMotorEncoder[motorA];
        int b = nMotorEncoder[motorB];
        int c = nMotorEncoder[motorC];

        // refresh display only if something changed
        if (a != lastA || b != lastB || c != lastC){
            displayBigTextLine(2, "A: %d", a);
            displayBigTextLine(4, "B: %d", b);
            displayBigTextLine(6, "C: %d", c);
            lastA = a;
            lastB = b;
            lastC = c;
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
