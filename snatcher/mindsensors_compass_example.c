#pragma config(Sensor, S1,     MSIMU,                sensorI2CCustomFastSkipStates)


#include "MindSensors_imu.h"


task main(){
	short CompassHeading = 0;

  eraseDisplay();

	int LastX     = 31; // out of range to force initial display
	int LastY     = 31; // out of range to force initial display

  nxtDrawEllipse(0, 62, 62, 0);
	nxtDisplayStringAt(29, 58, "N");
	nxtDisplayStringAt(29,  8, "S");
	nxtDisplayStringAt( 3, 34, "W");
	nxtDisplayStringAt(54, 34, "E");
  nxtDrawLine(31, 31, LastX, LastY);

  int X;
  int Y;

	while(true){
		CompassHeading = MSIMUreadHeading(MSIMU);
		CompassHeading = CompassHeading % 360;
	  if (CompassHeading < 0)
	  	CompassHeading += 360;

	  X = 31 + (float) (sinDegrees(CompassHeading) * 31);
	  Y = 31 + (float) (cosDegrees(CompassHeading) * 31);
	  	if ((LastX != X)  || (LastY != Y)){
	    	nxtInvertLine(31, 31, X,  Y);
				nxtInvertLine(31, 31, LastX, LastY);
				nxtDisplayStringAt(57, 60, "Heading");
	      nxtDisplayStringAt(65, 52, "%+5d", CompassHeading);
	      LastX = X;
	      LastY = Y;
	   	}
		wait1Msec(50);
	}

}
