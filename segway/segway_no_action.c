const tSensors Gyro = S2;
const float your_wheel_diameter = 42;

#include "segway_driver.h"

task main()
{
  //Start balancing and wait for configuration to finish
  StartTask(balancing);
  while(starting_balancing_task){}
}
