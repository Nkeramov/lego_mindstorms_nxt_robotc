void SearchAngle(float *x, float *y, int angle_max = 90, int speed = 10){
	angle_max = abs(angle_max);
	speed = abs(speed);
	int angle_min = 0;
	int dist_new = 0;
	int dist_min = 255;
	int angle_old = 0;
	Rotate(-angle_max,speed);
	wait1Msec(300);
	gyroAngle = -angle_max;
	nSyncedMotors = synchBC;
	nSyncedTurnRatio = -100;
	motor[motorB] = -speed;
	while (gyroAngle<angle_max){
		dist_new = GetDistance();
		if (dist_new < dist_min) {
			dist_min = dist_new;
			angle_min = ceil(gyroAngle + angle_old);
		}
	}
	OXAngle=90-angle_max;
	ResetMotor(motorB);
	ResetMotor(motorC);
	wait1Msec(300);
	//Rotate(ceil(-gyroAngle), speed);
	*x = dist_min * cosDegrees(angle_min);
	//distance from claw (color sensor) to backward of sonar
	*y = dist_min * sinDegrees(angle_min);
}