task main()
{
	eraseDisplay();

	int encB0 = nMotorEncoder[motorB];
	int encC0 = nMotorEncoder[motorC];
	int D = 56;
	int L = 100; // cm
	float path = (L * 10) / (PI * D) * 360;
	motor[motorB] = 50;
	motor[motorC] = 50;
	while((abs(nMotorEncoder[motorB] - encB0) < path) || (abs(nMotorEncoder[motorC] - encC0) < path)){
		int encB = abs(nMotorEncoder[motorB] - encB0);
		int encC = abs(nMotorEncoder[motorC] - encC0);
		int err = encC - encB;
		displayBigTextLine(0, "e =%d", abs(err));
		displayBigTextLine(2, "l =%d", encB);
		displayBigTextLine(4, "r =%d", encC);
		displayBigTextLine(6, "p =%d", path);
		int u = 0;
		if (abs(err) > 5){
			u = err * 2;
			int mB = 50 + u;
			int mC = 50 - u;
			if (mB > 100) mB = 100;
			if (mC > 100) mC = 100;
			if (mB < 0) mB = 0;
			if (mC < 0) mC = 0;
			if (encB >= path) mB = 0;
			if (encC >= path) mC = 0;
			motor[motorB] = mB;
			motor[motorC] = mC;
		}
		wait1Msec(10);
	}
	motor[motorB] = 0;
	motor[motorC] = 0;
	playSound(soundBeepBeep);
}
