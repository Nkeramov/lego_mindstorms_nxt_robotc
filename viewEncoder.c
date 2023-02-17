task main()
{
	nMotorEncoder[motorA] = 0;
	nMotorEncoder[motorB] = 0;
	nMotorEncoder[motorC] = 0;
	while(true){
		eraseDisplay();
		displayCenteredBigTextLine(0,"enc-view");
		displayBigTextLine(2,"A - %d",nMotorEncoder[motorA]);
		displayBigTextLine(4,"B - %d",nMotorEncoder[motorB]);
		displayBigTextLine(6,"C - %d",nMotorEncoder[motorC]);
		wait1Msec(50);
	}
}
