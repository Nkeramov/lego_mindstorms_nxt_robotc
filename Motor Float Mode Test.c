////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                    Motor Braking Test
//
// It turns out it draws about 25 mA of current per motor to hold it in a brake mode even when
// it is not moving. So the ROBOTC firmware will not activate "Brake" mode until an application
// program has actually used a motor. A motor is 'used' when a non-zero speed has been applied
// to the robot.
//
// Following program can be used to test this.simply try to turn the motor by hand and notice
// the increased pressure after the tone.
//
////////////////////////////////////////////////////////////////////////////////////////////////////

task main()
{
	const int kStepDuration = 3000;

	bFloatDuringInactiveMotorPWM = true;

	// Motor is in "float" mode when stopped until the motor has been used

	playSound(soundBeepBeep);

	// Program "uses" the motor -- i.e. any non-zero powr setting

	motor[motorA] = 100;

	// Stop the motor -- it will now be it 'brake' mode
wait1Msec(kStepDuration);
	motor[motorA] = 0;
		playSound(soundBeepBeep);
		wait1Msec(kStepDuration);
}
