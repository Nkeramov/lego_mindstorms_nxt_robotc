#pragma config(Sensor, S1,     MSIMU,                sensorI2CCustomFastSkipStates)

const 	tSensors gyroPort = S1;

#include "MindSensors_imu.h"
#include "MindSensors_gyroDriver.c"

float old_gyroAngle = -1;

task main(){

	short x_val, y_val, z_val;

  displayCenteredTextLine(0, "Mindsensors");
  displayCenteredBigTextLine(1, "IMU");
  displayCenteredTextLine(3, "Example");
  displayCenteredTextLine(5, "Connect sensor");
  displayCenteredTextLine(6, "to S1");
  wait1Msec(2000);
  eraseDisplay();
  while (gyroState == 0);
	old_gyroAngle = -100;
	while(true){
		if (gyroAngle != old_gyroAngle){
			eraseDisplay();
			displayCenteredTextLine(0, "Mindsensors");
			displayCenteredTextLine(2, "IMU gyro");
			displayCenteredBigTextLine(4, "%.3f", gyroAngle);
			old_gyroAngle = gyroAngle;
		}
		wait1Msec(5);
	}
}
