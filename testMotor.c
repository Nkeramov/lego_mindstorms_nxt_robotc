task main()
{
	motor[motorA] = 0;
	nMotorEncoder[motorA] = 0;
	nMotorEncoderTarget[motorA] = 720;
	motor[motorA] = 25;
	while(nMotorRunState[motorA] != runStateIdle);
	playSound(soundBlip);
	while(bSoundActive);
}
