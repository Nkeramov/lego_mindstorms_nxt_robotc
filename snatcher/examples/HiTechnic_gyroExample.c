#pragma config(Sensor, S1,     gyro,           sensorI2CHiTechnicGyro)

const 	tSensors gyroPort = S1;

#include "HiTechnic_new_gyroDriver.c"

float old_gyroAngle = -1;

task main(){
  displayCenteredTextLine(0, "HiTechnic");
  displayCenteredBigTextLine(1, "gyro");
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
			displayCenteredTextLine(0, "HiTechnic");
			displayCenteredTextLine(2, "gyro");
			displayCenteredBigTextLine(4, "%.3f", gyroAngle);
			old_gyroAngle = gyroAngle;
		}
		wait1Msec(5);
	}
}
