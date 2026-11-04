#pragma config(Sensor, S1,     transporter,    sensorTouch)
#pragma config(Sensor, S2,     pusher,         sensorTouch)
#pragma config(Sensor, S3,     color,          sensorColorNxtFULL)


int red = 0, green = 0, blue = 0, yellow = 0;
int currentTransporterEncoder = 0;
int centerTransporterEncoder = 300;
int redTransporterEncoder = 270;
int greenTransporterEncoder = 270 - 180;
int blueTransporterEncoder = 270 - 360;
int yellowTransporterEncoder = 270 - 540;


void initTransporter(){
	motor[motorB] = -30;
	while (SensorValue[S1] == 0){
		wait1Msec(5);
	}
	motor[motorB] = 0;
	nMotorEncoder[motorB] = 0;
	nMotorEncoderTarget[motorB] = centerTransporterEncoder;
	motor[motorB] = 30;
	while(nMotorRunState[motorB] != runStateIdle){
		wait1Msec(5);
	}
	nMotorEncoder[motorB] = 0;
	motor[motorB] = 0;
	currentTransporterEncoder = 0;
}

void moveToTargetPosition(int targetPosition){
	int encoderDifference = targetPosition - currentTransporterEncoder;
	if (encoderDifference != 0) {
		nMotorEncoder[motorB] = 0;
		nMotorEncoderTarget[motorB] = abs(encoderDifference);
		motor[motorB] = sgn(encoderDifference) * 30;
		while(nMotorRunState[motorB] != runStateIdle){
			wait1Msec(5);
		}
		currentTransporterEncoder = targetPosition;
	}
	motor[motorB] = 0;
}

void pushCube(){
	motor[motorA] = -40;
	wait1Msec(500);
	while (SensorValue[S2] == 0){
		wait1Msec(5);
	}
	motor[motorA] = 0;
}

void printInfo(){
	displayBigTextLine(0, "Red %d", red);
	displayBigTextLine(2, "Green %d", green);
	displayBigTextLine(4, "Blue %d", blue);
	displayBigTextLine(6, "Yellow %d", yellow);
}

task main(){
	initTransporter();
	while(true){
		TColors brickColor = SensorValue[color];
		if (brickColor != BLACKCOLOR){
			motor[motorC] = -100;
			switch(brickColor){
				case REDCOLOR:
					red++;
					moveToTargetPosition(redTransporterEncoder);
					break;
				case GREENCOLOR:
					green++;
					moveToTargetPosition(greenTransporterEncoder);
					break;
				case BLUECOLOR:
					blue++;
					moveToTargetPosition(blueTransporterEncoder);
					break;
				case YELLOWCOLOR:
					yellow++;
					moveToTargetPosition(yellowTransporterEncoder);
					break;
			}
			printInfo();
			pushCube();
			wait1Msec(1500);
		}
		else
			motor[motorC] = 0;
	}
}
