#pragma config(Sensor, S1,     conveyor,       sensorTouch)
#pragma config(Sensor, S2,     pusher,         sensorTouch)
#pragma config(Sensor, S3,     ,               sensorColorNxtFULL)

int blue = 0, red = 0, green = 0, yellow = 0;

void moveCenter(){
	motor[motorB] = -30;
	while (SensorValue[S1] == 0);
	motor[motorB] = 0;
	motor[motorB] = 30;
	wait1Msec(675);
	motor[motorB] = 0;
}

void pushCube(){
	motor[motorA] = -40;
	wait1Msec(500);
	while (SensorValue[S2] == 0);
	motor[motorA] = 0;
}

void printInfo(){
	displayBigTextLine(0, "Red %d", red);
	displayBigTextLine(2, "Green %d", green);
	displayBigTextLine(4, "Blue %d", blue);
	displayBigTextLine(6, "Yellow %d", yellow);
}

void turnRight(){
	motor[motorB] = -40;
	wait1Msec(500);
	motor[motorB] = 0;
}

void turnLeft(){
	motor[motorB] = 40;
	wait1Msec(500);
	motor[motorB] = 0;
}

task main(){
	moveCenter();
	TColors newColor = BLACKCOLOR, oldColor = GREENCOLOR;
	while(true){
		newColor = SensorValue[S3];
		if (newColor != BLACKCOLOR){
			motor[motorC] = -100;
			switch(newColor){
				case REDCOLOR:
					red++;
					switch (oldColor){
						case GREENCOLOR: turnRight(); break;
						case REDCOLOR: break;
						case BLUECOLOR: turnRight(); turnRight(); break;
						case YELLOWCOLOR: turnRight(); turnRight(); turnRight(); break;
					}
					break;
				case GREENCOLOR:
					green++;
					switch (oldColor){
						case GREENCOLOR: break;
						case REDCOLOR: turnLeft(); break;
						case BLUECOLOR: turnRight(); break;
						case YELLOWCOLOR: turnRight(); turnRight(); break;
					}
					break;
				case BLUECOLOR:
					blue++;
					switch (oldColor){
						case GREENCOLOR: turnLeft(); break;
						case REDCOLOR: turnLeft(); turnLeft(); break;
						case BLUECOLOR: break;
						case YELLOWCOLOR: turnRight(); break;
					}
					break;
				case YELLOWCOLOR:
					yellow++;
					switch (oldColor){
						case GREENCOLOR: turnLeft(); turnLeft(); break;
						case REDCOLOR: turnLeft(); turnLeft(); turnLeft(); break;
						case BLUECOLOR: turnLeft(); break;
						case YELLOWCOLOR: break;
					}
					break;
			}
			printInfo();
			pushCube();
			wait1Msec(1500);
		}
		else
			motor[motorC] = 0;
		oldColor = newColor;
	}
}
