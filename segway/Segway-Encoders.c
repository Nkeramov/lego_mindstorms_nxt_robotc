const tSensors Gyro = S2;
const float your_wheel_diameter = 42;
#include "segway-driver.h"

task main()
{
  //Start balancing and wait for configuration to finish
  StartTask(balancing);
  while(starting_balancing_task){}

  while(true)
  {
	  speed = 60;
	  while(nMotorEncoder[motorB] > -1000){wait1Msec(100);}
	  //#1: Polling 10 times/sec is good enough -- Save processor power for the balancing task.
	  //#2: Going forwards means that the motors run backwards, because of their orientation.
	  //    So to go forward for 1000 degrees, the encoder must reach -1000 degrees.
	  speed = -60;
	  while(nMotorEncoder[motorB] < 0){wait1Msec(100);}
  }

}
