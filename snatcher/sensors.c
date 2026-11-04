#pragma platform(NXT)
#pragma autoStartTasks

#define SONAR_SENSOR_REPEATS					5
#define SONAR_SENSOR_DELAY						50
#define SONAR_CORRECTION_OFFSET   		3
#define SONAR_CORRECTION_THRESHOLD 		25
#define SONAR_MAX_DISTANCE						255

#define COLOR_SENSOR_REPEATS					5
#define	COLOR_SENSOR_DELAY						50

TColors curColor = BLACKCOLOR, oldColor = BLACKCOLOR;
ubyte curDistance = 0, oldDistance = 0;

void ShellSort(int *a, int n){
	int d = n / 2;
	while (d > 0){
		for (int i = 0; i < n - d; i++){
			int j = i;
			while (j >= 0 && a[j] > a[j + d]){
				int t = a[j + d];
				a[j + d] = a[j];
				a[j] = t;
				j--;
			}
		}
		d = d / 2;
	}
}

ubyte getDistance(){
	int validReadings[SONAR_SENSOR_REPEATS];
  int validCount = 0;
  while (validCount < SONAR_SENSOR_REPEATS){
  	int val = SensorValue[sonar];
    if (val >= 0 && val <= SONAR_MAX_DISTANCE) {
    	validReadings[validCount++] = val;
    }
    wait1Msec(SONAR_SENSOR_DELAY);
  }
	ShellSort(validReadings, SONAR_SENSOR_REPEATS);
	int r = validReadings[SONAR_SENSOR_REPEATS / 2];
	if (SONAR_SENSOR_REPEATS % 2 == 0)
		r = round((validReadings[SONAR_SENSOR_REPEATS / 2] + validReadings[SONAR_SENSOR_REPEATS / 2 + 1]) / 2.0);
	//if (r >= SONAR_CORRECTION_THRESHOLD)
	//	r += SONAR_CORRECTION_OFFSET;
	return (ubyte)clamp(r, 0, 255);
}

TColors getColor(){
	int counts[6] = {0, 0, 0, 0, 0, 0};
	int colors[6] = {BLACKCOLOR, BLUECOLOR, GREENCOLOR, YELLOWCOLOR, REDCOLOR, WHITECOLOR};
	for (int i = 0; i < COLOR_SENSOR_REPEATS; i++)
		switch (SensorValue[color]){
			case BLACKCOLOR:  counts[0]++; break;
			case BLUECOLOR:   counts[1]++; break;
			case GREENCOLOR:  counts[2]++; break;
			case YELLOWCOLOR: counts[3]++; break;
			case REDCOLOR:    counts[4]++; break;
			case WHITECOLOR:  counts[5]++; break;
			default:          counts[0]++; break;
		}
		wait1Msec(COLOR_SENSOR_DELAY);
	int k = 0;
	for (int i = 1; i < 6; i++){
		if (counts[i] > counts[k])
			k = i;
	}
	return colors[k];
}

task ColorSensorTask()
{
	nSchedulePriority = kHighPriority - 5;
	while (true)
	{
		curColor = getColor();
		wait1Msec(COLOR_SENSOR_DELAY);
	}
}


task DistanceSensorTask()
{
	nSchedulePriority = kHighPriority - 5;
	while (true)
	{
		curDistance = getDistance();
		wait1Msec(SONAR_SENSOR_DELAY);
	}
}
