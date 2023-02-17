#pragma platform(NXT)
#pragma autoStartTasks

int 		gyroState = 0;
float 	gyroAngle = 0.0;
float 	gyroBias = 0.0;
float 	dt=0.01;

#define gyroPort S2

const 	int GYRO_BIAS_SAMPLES = 400;

task GyroDriver()
{
	float gyroVelocity;
	long samplesSum = 0;

	nSchedulePriority = kHighPriority - 8;
	wait1Msec(1500);
	for (int i = 0; i < GYRO_BIAS_SAMPLES ; i++)
	{
		samplesSum += SensorRaw[gyroPort];
		wait1Msec(5);
	}
	gyroBias = (float) samplesSum / GYRO_BIAS_SAMPLES;
	gyroState = 1;
	clearTimer(T4);
	while (true){
		gyroVelocity = SensorRaw[gyroPort] - gyroBias;
		if (abs(gyroVelocity) > 2)
			gyroAngle += gyroVelocity * dt;

		while(time1[T4] < dt*1000) wait1Msec(1);
  	clearTimer(T4);
	}
}
