#pragma platform(NXT)
#pragma autoStartTasks

int 		gyroState = 0;
float 	gyroAngle = 0.0;
float 	dt = 0.01;

#define gyroPort S2

task GyroDriver()
{
	short x_val, y_val, z_val;
	float gyroVelocity = 0.0;

	nSchedulePriority = kHighPriority - 8;
	wait1Msec(1500);
	//MSIMUsetGyroFilter(gyroPort, 0x00);
	MSIMUsetGyroFilter(gyroPort, 0x04);
	wait1Msec(1000);
	gyroState = 1;
	clearTimer(T4);
	while (true){
		MSIMUreadGyroAxes(gyroPort, x_val, y_val, z_val);
		gyroVelocity = z_val * dt * 9 / 1000;
		if (abs(gyroVelocity) > 0.005)
  		gyroAngle = gyroAngle + gyroVelocity;
		while(time1[T4] < dt * 1000) wait1Msec(1);
  	clearTimer(T4);
	}
	return;
}
