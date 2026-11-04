#pragma platform(NXT)
#pragma autoStartTasks

#define GYRO_PORT 											S2
#define GYRO_BIAS_SAMPLES								100

typedef enum TGyroStates
{
	stateGyroInit,
	stateGyroWorking,
} TGyroStates;

TGyroStates 	gyroState = stateGyroInit;
float 				gyroAngle = 0.0;
float 				gyroBias = 0.0;
int 					samplingTimeMs = 10;					// NXT  analog sensor values are scanned by h/w every 3 msec


task GyroDriver()
{
	nSchedulePriority = kHighPriority - 8;
	wait1Msec(1500);
	long samplesSum = 0;
	for (int i = 0; i < GYRO_BIAS_SAMPLES ; i++)
	{
		samplesSum += SensorRaw[GYRO_PORT];
		wait1Msec(samplingTimeMs);
	}
	gyroBias = (float) samplesSum / GYRO_BIAS_SAMPLES;
	gyroState = stateGyroWorking;
	clearTimer(T1);
	while (true){
		float gyroVelocity = SensorRaw[GYRO_PORT];
		wait1Msec(3);
    gyroVelocity += SensorRaw[GYRO_PORT];
		gyroVelocity = gyroVelocity / 2 - gyroBias;
		if (fabs(gyroVelocity) > 1)
			gyroAngle += gyroVelocity * samplingTimeMs;
		while(time1[T1] < samplingTimeMs) {
			wait1Msec(1);
		}
  	clearTimer(T1);
	}
}
