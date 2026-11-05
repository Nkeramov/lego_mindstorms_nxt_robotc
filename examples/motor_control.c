int clampPower(int val, int min, int max) {
	return (val < min) ? min : ((val > max) ? max : val);
}

task main(){
	int k = 50;
	int lastBtn = kNoButton;
	while(true){
		int btn = nNxtButtonPressed;
		if (btn != lastBtn){
			if (btn == kLeftButton)  k -= 5;
			if (btn == kRightButton) k += 5;
			if (btn == kEnterButton) k = 0;
			k = clampPower(k, -100, 100);
		}
			motor[motorA] = k;
			motor[motorB] = k;
			motor[motorC] = k;
			eraseDisplay();
			displayCenteredBigTextLine(1, "speed", k);
			displayCenteredBigTextLine(3, "k = %d", k);
			if (k == 0)
				displayCenteredBigTextLine(6, "stopped");
			else if (k == 100)
				displayCenteredBigTextLine(6, "max");
			else if (k == -100)
				displayCenteredBigTextLine(6, "min");
		lastBtn = btn;
		wait1Msec(20);
	}
}
