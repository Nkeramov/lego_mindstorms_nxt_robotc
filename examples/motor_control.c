int lastBtn = kNoButton;

int clampPower(int v){
    if (v > 100) return 100;
    if (v < -100) return -100;
    return v;
}

task main(){
	while(true){
		int btn = nNxtButtonPressed;

		if (btn != lastBtn){
			if (btn == kLeftButton)  k -= 5;
			if (btn == kRightButton) k += 5;
			if (btn == kEnterButton) k = 0;

			clampPower(k);

			motor[motorA] = k;
			motor[motorB] = k;
			motor[motorC] = k;

			eraseDisplay();
			displayCenteredBigTextLine(3, "k = %d", k);
		}

		lastBtn = btn;
		wait1Msec(20);
	}
}