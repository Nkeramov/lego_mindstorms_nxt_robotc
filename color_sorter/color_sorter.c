#pragma config(Sensor, S1,     transporter,    sensorTouch)
#pragma config(Sensor, S2,     pusher,         sensorTouch)
#pragma config(Sensor, S3,     color,          sensorColorNxtFULL)
#pragma config(Motor,  motorA,          transporter,   tmotorNXT, PIDControl, encoder)
#pragma config(Motor,  motorB,          pusher,        tmotorNXT, PIDControl, encoder)
#pragma config(Motor,  motorC,          belt,          tmotorNXT, PIDControl, encoder)

#define BELT_SPEED 							100
#define PUSHER_SPEED						40
#define TRANSPORTER_SPEED   				30


int redBricksCnt = 0;
int greenBricksCnt = 0;
int blueBricksCnt = 0;
int yellowBricksCnt = 0;

int currentTransporterPosition = 0;
// encoder value for rotating the transporter from the far-right position to the center (point 0)
int centerTransporterPosition = 300;
// the following encoder values are calculated from the center (point 0) to the corresponding color
int redTransporterPosition = 270;
int greenTransporterPosition = 270 - 180;
int blueTransporterPosition = 270 - 360;
int yellowTransporterPosition = 270 - 540;


void initTransporter(){
	nMotorEncoder[transporter] = 0;
	motor[transporter] = -TRANSPORTER_SPEED;
	while (SensorValue[transporter] == 0){
		wait1Msec(5);
	}
	nMotorEncoder[transporter] = 0;
	nMotorEncoderTarget[transporter] = centerTransporterPosition;
	motor[transporter] = TRANSPORTER_SPEED;
	while(nMotorRunState[transporter] != runStateIdle){
		wait1Msec(5);
	}
	motor[transporter] = 0;
	nMotorEncoder[transporter] = 0;
	currentTransporterPosition = 0;
}

void moveToTargetPosition(int targetPosition){
	int encoderDifference = targetPosition - currentTransporterPosition;
	if (encoderDifference != 0) {
		nMotorEncoder[transporter] = 0;
		nMotorEncoderTarget[transporter] = abs(encoderDifference);
		motor[transporter] = sgn(encoderDifference) * TRANSPORTER_SPEED;
		while(nMotorRunState[transporter] != runStateIdle){
			wait1Msec(5);
		}
		currentTransporterPosition = targetPosition;
	}
	motor[transporter] = 0;
}

void pushCube(){
	motor[pusher] = -PUSHER_SPEED;
	while (SensorValue[pusher] == 0){
		wait1Msec(5);
	}
	motor[pusher] = 0;
}

void printInfo(){
	displayBigTextLine(0, "Red %d", redBricksCnt);
	displayBigTextLine(2, "Blue %d", blueBricksCnt);
	displayBigTextLine(4, "Green %d", greenBricksCnt);
	displayBigTextLine(6, "Yellow %d", yellowBricksCnt);
}

task main(){
	initTransporter();
	while(true){
		TColors brickColor = SensorValue[color];
		if (brickColor != BLACKCOLOR){
			motor[belt] = -BELT_SPEED;
			switch(brickColor){
				case REDCOLOR:
					redBricksCnt++;
					moveToTargetPosition(redTransporterPosition);
					break;
				case GREENCOLOR:
					greenBricksCnt++;
					moveToTargetPosition(greenTransporterPosition);
					break;
				case BLUECOLOR:
					blueBricksCnt++;
					moveToTargetPosition(blueTransporterPosition);
					break;
				case YELLOWCOLOR:
					yellowBricksCnt++;
					moveToTargetPosition(yellowTransporterPosition);
					break;
			}
			pushCube();
			printInfo();
			wait1Msec(1500);
		}
		else {
			motor[belt] = 0;
		}
	}
}
