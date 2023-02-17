#pragma config(Sensor, S1, touch, sensorTouch)
#pragma config(Sensor, S2, gyro, sensorI2CHiTechnicGyro)


float 	gyroAngle;
float 	gyroBias;
float 	dt = 0.01;
const 	int GYRO_BIAS_SAMPLES = 400;
task calcAngle()
{
	clearTimer(T4);
	while (true)
	{
		float gyroVelocity = SensorRaw[gyro] - gyroBias;
		if (abs(gyroVelocity) > 5)
			gyroAngle += gyroVelocity * dt;
		while(time1[T4] < dt*1000) wait1Msec(1);
  	clearTimer(T4);
	}
}


task viewAngle()
{
	while(true)
	{
		eraseDisplay();
		displayCenteredBigTextLine(2, "angle");
		displayCenteredBigTextLine(4, "%3.2f",  gyroAngle);
		wait1Msec(50);
	}
}


void rotate(int angle, int speed = 15)
{
	nSyncedMotors = synchBC;
	nSyncedTurnRatio = -100;
	float targetAngle = gyroAngle + angle;
	if (angle < 0)
	{
		motor[motorB] = speed;
		while (gyroAngle > targetAngle){
			wait1Msec(5);
		}
	}
	else
	{
		motor[motorB] = -speed;
		while (gyroAngle < targetAngle){
			wait1Msec(5);
		}
	}
	motor[motorB] = 0;
}

task main()
{
	wait1Msec(5000);
	startTask(viewAngle);
	playSound(soundBlip);
	int samplesSum = 0;
	for (int i = 0; i < GYRO_BIAS_SAMPLES ; i++)
	{
		samplesSum += SensorRaw[gyro];
		wait1Msec(5);
	}
	gyroBias = (float) samplesSum / GYRO_BIAS_SAMPLES;
	gyroAngle = 0;
	startTask(calcAngle);

	rotate(-90)
	wait1Msec(1000);
	while (SensorValue[touch] == 0);
		rotate(90)
	while (SensorValue[touch] == 1);
	//playSound(soundBlip);
	while(true);
}
