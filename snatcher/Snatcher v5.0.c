	#pragma config(Sensor, S1, touch, sensorTouch)
	#pragma config(Sensor, S2, gyro, sensorI2CHiTechnicGyro)
	#pragma config(Sensor, S3, color, sensorColorNxtFULL)
	#pragma config(Sensor, S4, sonar, sensorSONAR)
// ================================================================
	//#define MindSensors_IMU		// #pragma config(Sensor, S2, gyro, sensorI2CCustomFastSkipStates)
	#define HiTechnic_Gyro 			// #pragma config(Sensor, S2, gyro, sensorI2CHiTechnicGyro)

	#define I2C_Touch				// #pragma config(Sensor, S1, touch, sensorI2CCustom)
	//#define LEGO_Touch				// #pragma config(Sensor, S1, touch, sensorTouch)

	#ifdef HiTechnic_Gyro
			#include "HiTechnic_new_gyroDriver.c"
	#endif
	#ifdef MindSensors_IMU
			#include "MindSensors_imu.h"
			#include "MindSensors_gyroDriver.c"
	#endif
// ================================================================
	const int MOVE_POINTS = 5;
	const int SONAR_REPEAT = 5;
	const int COLOR_REPEAT = 5;
	const int MAX_VELOCITY = 50;
	const int BT_DATA_SIZE = 8;
	const int BT_MASTER_MODE = 1;
	const int BT_SLAVE_MODE = 2;
	const float FRICTION_VAL = 0.02899;
	//0.037 - wheels
	//0.029 - caterpillars
	//0.02842	printed
	//0.03944 new
	// 4 buttons, claw state, speed and 2 reserved bits
	ubyte bstate[BT_DATA_SIZE] = {0,  0,  0,  0,  1,  50, 0, 0};
	ubyte old_bstate[BT_DATA_SIZE] = {0,  0,  0,  0,  1,  50, 0, 0};
	TNxtButtons curBtn = kNoButton,  oldBtn = kNoButton;
	TColors curColor = BLACKCOLOR,  oldColor = BLACKCOLOR;
	int curDistance = 0,  oldDistance = 0, OXAngle = 90;
	bool claw = true,  clawState = false;
	float curX = 0,  curY = 0;
	int kOld = 0,  kNew = 0;
// ================================================================
void MoveFwd(short speed = 30){
	nSyncedTurnRatio = 100;
	nSyncedMotors = synchBC;
	motor[motorB] = speed;
}
// ================================================================
void Turn(short speed = 50){
	nSyncedMotors = synchBC;
	nSyncedTurnRatio = -100;
	motor[motorB] = speed;
}
// ================================================================
void FwdLeft(short speed = 50){
	nSyncedMotors = synchBC;
	nSyncedTurnRatio = 75;
	motor[motorB] = speed;
}
// ================================================================
void FwdRight(short speed = 50){
	nSyncedMotors = synchCB;
	nSyncedTurnRatio = 75;
	motor[motorC] = speed;
}
// ================================================================
void ShellSort(int *a, int n){
	int d = 2;
	while (d > 0){
		for (int i = 0; i < n - d; i++){
			int j = i;
			while (a[j] > a[j + d] && j >= 0){
				int t = a[j + d];
				a[j + d] = a[j];
				a[j] = t;
				j--;
			}
		}
		d = d / 2;
	}
}
// ================================================================
int GetDistance(){
	int a[SONAR_REPEAT];
	for (int i = 0; i < SONAR_REPEAT; i++){
		a[i] = SensorValue[sonar];
		wait1Msec(1);
	}
	ShellSort(a, SONAR_REPEAT);
	int r = 0;
	if (SONAR_REPEAT % 2 == 0)
		r = round((a[SONAR_REPEAT / 2] + a[SONAR_REPEAT / 2 + 1]) / 2);
	else
		r = a[SONAR_REPEAT / 2];
	if (r >= 25)
		r += 3;
	return r;
}
// ================================================================
TColors GetColor(){
	int a[6] = {0, 0, 0, 0, 0, 0}; // 6 colors : black, blue, green, yellow, red, white
	for (int i = 0; i < COLOR_REPEAT; i++)
		switch (SensorValue[color]){
			case BLACKCOLOR:  a[0]++; break;
			case BLUECOLOR:   a[1]++; break;
			case GREENCOLOR:  a[2]++; break;
			case YELLOWCOLOR: a[3]++; break;
			case REDCOLOR:    a[4]++; break;
			case WHITECOLOR:  a[5]++; break;
			default:          a[0]++; break;
		}
	int k = 0;
	for (int i = 1; i < 6; i++){
		if (a[i] > a[k])
			k = i;
	}
	TColors c = BLACKCOLOR;
	switch (k){
		case 0:  c = BLACKCOLOR; break;
		case 1:  c = BLUECOLOR; break;
		case 2:  c = GREENCOLOR; break;
		case 3:  c = YELLOWCOLOR; break;
		case 4:  c = REDCOLOR; break;
		case 5:  c = WHITECOLOR; break;
		default: c = BLACKCOLOR; break;
	}
	return c;
}
// ================================================================
int GetTouch(){
	#ifdef I2C_Touch
		SensorType[touch] = sensorTouch;
		wait1Msec(50);
		int a = SensorValue[touch];
		SensorType[S1] = sensorI2CCustom;
	#endif
	#ifdef LEGO_Touch
		int a = SensorValue[touch];
	#endif
	return a;
}
// ================================================================
void ResetMotor(tMotor mname){
	if (mname != motorA)
		nSyncedMotors = synchNone;
	motor[mname] = 0;
	nMotorEncoder[mname] = 0;
	nMotorEncoderTarget[mname] = 0;
}
// ================================================================
void Stop(){
	nSyncedMotors = synchNone;
	motor[motorB] = 0;
	nMotorEncoder[motorB] = 0;
	nMotorEncoderTarget[motorB] = 0;
	motor[motorC] = 0;
	nMotorEncoder[motorC] = 0;
	nMotorEncoderTarget[motorC] = 0;
}
// ================================================================
void MoveStraight(int dist,  short speed = 40){
	if (dist < 0){
		speed = -abs(speed);
		dist = abs(dist);
	}
	Stop();
	float old_gyroAngle = gyroAngle;
	/*
	nSyncedMotors = synchBC;
	nSyncedTurnRatio = 100;
	motor[motorB] = speed;
	while (abs(nMotorEncoder[motorB]) * FRICTION_VAL < dist);
	*/
	int m1 = speed; int m2 = speed;
	motor[motorB] = m1;
	motor[motorC] = m2;
	eraseDisplay();

	while (abs(nMotorEncoder[motorB]) * FRICTION_VAL < dist){
		float e = gyroAngle - old_gyroAngle;
		//old_gyroAngle = gyroAngle;
		if (abs(e) >= 2.0){
			m1 = round(speed + e * 3.0);
			m2 = round(speed - e * 3.0);
			if (m1 > speed * 2) m1 = speed * 2;
			if (m2 > speed * 2) m2 = speed * 2;
			if (m1 < speed / 2) m1 = speed / 2;
			if (m2 < speed / 2) m2 = speed / 2;
		} else {
			m1 = speed;
			m2 = speed;
		}
		motor[motorB] = m1;
		motor[motorC] = m2;
		wait1Msec(200);
	}
	//eraseDisplay();
	//displayBigTextLine(0, "%.2f",old_gyroAngle);
	//displayBigTextLine(4, "%.2f",gyroAngle);
	Stop();
	curX += sgn(speed) * dist * cosDegrees(OXAngle);
	curY += sgn(speed) * dist * sinDegrees(OXAngle);
}
// ================================================================
#ifdef I2C_Touch
	void SetRGB(TColors cleft,  TColors cright){
		byte i2cconfg[3];
		const tSensors kPort = S1;
		const short kSize = 0;
		const short kAddress = 1;
		const short kData = 2;
		typedef ubyte kResults[4];
		short nAddress = 0x40;
		i2cconfg[kSize] = 2;
		i2cconfg[kAddress] = nAddress;
		i2cconfg[kData] = 0b00011000;
		switch (cleft){
			case BLUECOLOR:   i2cconfg[kData] += 0b00000011; break;
			case GREENCOLOR:  i2cconfg[kData] += 0b00000101; break;
			case REDCOLOR:    i2cconfg[kData] += 0b00000110; break;
			default:          i2cconfg[kData] += 0b00000111; break;
		}
		switch (cright){
			case BLUECOLOR:   i2cconfg[kData] += 0b01100000; break;
			case GREENCOLOR:  i2cconfg[kData] += 0b10100000; break;
			case REDCOLOR:    i2cconfg[kData] += 0b11000000; break;
			default:          i2cconfg[kData] += 0b11100000; break;
		}
		sendI2CMsg(kPort,  &i2cconfg[0],  sizeof(kResults));
		while (nI2CStatus[kPort] == STAT_COMM_PENDING);
	}
#endif
// ================================================================
task Claw {
	while (true){
		if (claw && !clawState) {
			ResetMotor(motorA);
			motor[motorA] = 40;
			waitUntil(GetTouch());
			ResetMotor(motorA);
			wait1Msec(250);
			motor[motorA] = 15;
			clawState = true;
		} else
		if (!claw && clawState){
			ResetMotor(motorA);
			motor[motorA] = 40;
			waitUntil(GetTouch());
			ResetMotor(motorA);
			motor[motorA] = -40;
			waitUntil(abs(nMotorEncoder[motorA]) >= 600);
			ResetMotor(motorA);
			clawState = false;
		}

	}wait1Msec(250);
}
// ================================================================
void Rotate(float angle = 180, short speed = 20){
	Stop();
	OXAngle -= angle;
	speed = abs(speed);
	if (angle > 0)
		speed *= -1;
	angle = abs(angle);
	gyroAngle = 0;
	nSyncedMotors = synchBC;
	nSyncedTurnRatio = -100;
	motor[motorB] = speed;
	/*
	if (angle > 30) {
		waitUntil(abs(gyroAngle) >= angle - 30);
		motor[motorB] = round(speed * 2.0 / 3.0);
	}
	else motor[motorB] = round(speed * 2.0 / 3.0);
	*/
	waitUntil(abs(gyroAngle) >= angle);
	Stop();
}
// ================================================================
void SearchAngle(int *ang, int *dist, int angle_max = 90, short speed = 10){
	angle_max = abs(angle_max) % 360;
	speed = abs(speed);
	int angle_min = 0;
	int dist_new = 0;
	int dist_min = 255;
	int angle_old = 0;
	Rotate(-angle_max, speed);
	wait1Msec(300);
	gyroAngle = -angle_max;
	nSyncedMotors = synchBC;
	nSyncedTurnRatio = -100;
	motor[motorB] = -speed;
	while (gyroAngle < angle_max){
		dist_new = GetDistance();
		if (dist_new < dist_min) {
			dist_min = dist_new;
			angle_min = ceil(gyroAngle + angle_old);
		}
	}
	Stop();
	wait1Msec(300);
	Rotate(ceil(-gyroAngle),  speed);
	*ang = angle_min;
	//distance from claw (color sensor) to backward of sonar
	*dist = dist_min - 10;
}
// ================================================================
//r+  l-  OX Reversed
void MoveTo(float x,  float y){
	float dx = curX - x,  dy = curY - y;
	int length = sqrt(dx * dx + dy * dy);
	int delta = OXAngle;
	if (dx < 0)
			delta -= radiansToDegrees(atan(dy / dx));
		else if (dx > 0)
			delta -= radiansToDegrees(atan(dy / dx)) + 180;
			else
				delta += sgn(dy) * 90;
	delta = delta % 360;
	if (abs(delta) > 180)
		delta -= sgn(delta) * 360;
	Rotate(delta);
	wait1Msec(300);
	MoveStraight(length,  80);
}
// ================================================================
void TestSnatcher(){
	eraseDisplay();
	displayCenteredBigTextLine(2, "TESTING");
	MoveStraight(15, 50);
	wait1Msec(1000);
	MoveStraight(-15, 50);
	wait1Msec(1000);
	Rotate(180, 30);
	wait1Msec(1000);
	Rotate(-180, 30);
	wait1Msec(1000);
	claw = true;
	waitUntil(GetTouch());
	wait1Msec(1000);
	claw = false;
	playSound(soundBeepBeep);
	claw = true;
	waitUntil(GetTouch());
	Rotate(45, 30);
	wait1Msec(1000);
	Rotate(-90, 30);
	wait1Msec(1000);
	Rotate(45, 30);
	claw = false;
}
// ================================================================
void DrawBTlogo(int mode){
	drawLine(92 - 80, 63, 92 - 80, 45);
	drawLine(92 - 80, 63, 98 - 80, 57);
	drawLine(98 - 80, 57, 92 - 80, 54);
	drawLine(92 - 80, 54, 98 - 80, 51);
	drawLine(98 - 80, 51, 92 - 80, 45);
	drawLine(93 - 80, 62, 93 - 80, 46);
	drawLine(94 - 80, 61, 94 - 80, 47);
	drawLine(95 - 80, 60, 95 - 80, 48);
	drawLine(96 - 80, 59, 96 - 80, 49);
	drawLine(97 - 80, 58, 97 - 80, 50);
	invertLine(95 - 80, 54, 95 - 80, 54);
	invertLine(96 - 80, 53, 96 - 80, 55);
	invertLine(97 - 80, 53, 97 - 80, 55);
	drawTextAt(25, 60, "bluetooth");
	if (mode == BT_MASTER_MODE)
		drawTextAt(25, 52, "master mode");
	else
		if (mode == BT_SLAVE_MODE)
			drawTextAt(25, 52, "slave mode");
	drawLine(92 - 80, 53, 92 - 80 - 4, 51);
	drawLine(92 - 80, 52, 92 - 80 - 4, 50);
	drawLine(92 - 80, 55, 92 - 80 - 4, 57);
	drawLine(92 - 80, 56, 92 - 80 - 4, 58);
}
// ================================================================
short SelectProgram(void){
	short curProg = 0, selProg = -1;
	bool exit = false;
	nNxtExitClicks  = 2;
	eraseDisplay();
	displayCenteredBigTextLine(0, "SELECT");
	displayCenteredBigTextLine(2, "PROGRAM");
	displayCenteredBigTextLine(4, "%d", curProg);
	displayCenteredTextLine(6, "test motors");
	displayCenteredTextLine(7, "and claw");
	// selecting program
	while (selProg < 0){
		// wait for button press
		waitUntil(nNxtButtonPressed != kNoButton);
		curBtn = nNxtButtonPressed;
		playSound(soundBlip);
		switch (curBtn){
			case kLeftButton: curProg--; if (curProg < 0) curProg = 13;	break;
			case kRightButton:	curProg++; if (curProg > 13) curProg = 0;	break;
			case kEnterButton:	selProg = curProg; break;
			case kExitButton:	 exit = true;  break;
		}
		if (exit){
			eraseDisplay();
			displayCenteredBigTextLine(0, "FOR EXIT");
			displayCenteredBigTextLine(2, "PRESS");
			displayCenteredBigTextLine(4, "AGAIN");
			wait1Msec(1000);
			exit = false;
		}
		eraseDisplay();
		displayCenteredBigTextLine(0, "SELECT");
		displayCenteredBigTextLine(2, "PROGRAM");
		displayCenteredBigTextLine(4, "%d", curProg);
		switch (curProg){
			case 0:
				displayCenteredTextLine(6, "test motors");
				displayCenteredTextLine(7, "and claw");
				break;
			case 1:
				displayCenteredTextLine(6, "find and catch");
				displayCenteredTextLine(7, "nearest object");
				break;
			case 2:
				displayCenteredTextLine(6, "random moving");
				displayCenteredTextLine(7, "and returning");
				break;
			case 3:
				displayCenteredTextLine(6, "wall");
				displayCenteredTextLine(7, "avoidance");
				break;
			case 4:
				displayCenteredTextLine(6, "select green");
				displayCenteredTextLine(7, "and blue colors");
				break;
			case 5:
				displayCenteredTextLine(6, "view sonar");
				displayCenteredTextLine(7, "sensor");
				break;
			case 6:
				displayCenteredTextLine(6, "view gyro");
				displayCenteredTextLine(7, "sensor");
				break;
			case 7:
				displayCenteredTextLine(6, "view color");
				displayCenteredTextLine(7, "sensor");
				break;
			case 8:
				displayCenteredTextLine(6, "play all");
				displayCenteredTextLine(7, "sounds");
				break;
			case 9:
				displayCenteredTextLine(6, "view pressed");
				displayCenteredTextLine(7, "button");
				break;
			case 10:
				displayCenteredTextLine(6, "manual");
				displayCenteredTextLine(7, "control");
				break;
			case 11:
				displayCenteredTextLine(6, "Slave");
				break;
			case 12:
				displayCenteredTextLine(6, "find and catch");
				displayCenteredTextLine(7, "everything");
				break;
			case 13:
				displayCenteredTextLine(6, "BT control");
				displayCenteredTextLine(7, "Single");
				break;
		}
		// wait for button release
		waitUntil(nNxtButtonPressed == kNoButton);
	}
	// program selected go to perform it
	eraseDisplay();
	displayCenteredBigTextLine(2, "SELECTED");
	displayCenteredBigTextLine(4, "%d", selProg);
	wait1Msec(1000);
	eraseDisplay();
	return selProg;
}
// ================================================================
void InitSnatcher(){
	eraseDisplay();
	displayCenteredBigTextLine(0, "LEGO");
	displayCenteredBigTextLine(2, "SNATCHER");
	wait1Msec(250);
	eraseDisplay();
	startTask(Claw);
	displayCenteredBigTextLine(0, "LEGO");
	displayCenteredBigTextLine(2, "SNATCHER");
	displayCenteredBigTextLine(4, "InitGyro");
	playSound (soundBeepBeep);
	wait1Msec(500);
	while (!gyroState); // init gyro
	gyroAngle = 0;
	playSound (soundBeepBeep);
}
// ================================================================
task main()
{

	int speed = 0;
	setSessionPIN("1234");
	wait1Msec(1000);
	claw = true;
	float fOldAngle = -1;
	InitSnatcher();
		#ifdef I2C_Touch
		SetRGB(REDCOLOR,  REDCOLOR);
		wait1Msec(1000);
		SetRGB(GREENCOLOR,  GREENCOLOR);
		wait1Msec(1000);
		SetRGB(BLUECOLOR,  BLUECOLOR);
		wait1Msec(1000);
		SetRGB(BLACKCOLOR,  BLACKCOLOR);
	#endif
	claw = true; waitUntil(clawState);
	int numberProgram = SelectProgram();
	//int numberProgram = 6;
	switch (numberProgram){
	case 0: //self-test code
		TNxtButtons curBtnTest = kNoButton;
		bool test = true;
		while(test == true){
			TestSnatcher();
			eraseDisplay();
			displayCenteredBigTextLine(2, "REPEAT");
			displayCenteredBigTextLine(4, "TEST?");
			displayCenteredTextLine(6, "PRESS");
			displayCenteredTextLine(7, "ENTER/EXIT");
			while (nNxtButtonPressed == kNoButton);
			curBtnTest = nNxtButtonPressed;
			if (curBtnTest == kExitButton)
				test = false;
		}
		break;
	case 1: //searching objects
		int Angle = 0, Distance = 0;
		SearchAngle(&Angle,  &Distance,  90,  20);
		eraseDisplay();
		displayTextLine(0,  "Angle %d",  Angle);
		displayTextLine(2,  "Distance %d",  Distance);
		Rotate(Angle);
		claw = false; waitUntil(!clawState);
		MoveStraight(Distance, 50);
		claw = true; waitUntil(clawState);
		switch (GetColor()){
			case  BLACKCOLOR: displayCenteredBigTextLine(4, "Black"); playSoundFile("black.rso"); break;
			case  BLUECOLOR:	displayCenteredBigTextLine(4, "Blue"); playSoundFile("blue.rso"); break;
			case  GREENCOLOR:	displayCenteredBigTextLine(4, "Green"); playSoundFile("green.rso"); break;
			case  YELLOWCOLOR: displayCenteredBigTextLine(4, "Yellow"); playSoundFile("yellow.rso"); break;
			case  REDCOLOR: displayCenteredBigTextLine(4, "Red"); playSoundFile("red.rso"); break;
			case  WHITECOLOR: displayCenteredBigTextLine(4, "White"); playSoundFile("white.rso"); break;
			default: displayCenteredBigTextLine(4, "Undefined"); break;
		}
		break;
	case 2: //moving and returning
		#ifdef I2C_Touch
			SetRGB(REDCOLOR,  REDCOLOR);
		#endif
		float dx = 0, dy = 0;
		int dist, delta = 0, old_delta = 0;
		for(int i = 0; i < MOVE_POINTS; i++){
			old_delta = delta;
			while (delta == old_delta)
				delta = (abs(rand() % 5) - 2) * 45;  // from -90 to 90
			Rotate(delta);
			wait1Msec(250);
			dist = 512;
			// 10 - offset to obstacle
			while (dist > GetDistance() - 10)
				dist = (abs(rand() % 3) + 3) * 10;
			MoveStraight(dist);
			dx += cosDegrees(OXAngle) * dist;
			dy += sinDegrees(OXAngle) * dist;
			eraseDisplay();
			displayBigTextLine(0, "a = %d", OXAngle);
			displayBigTextLine(2, "dx = %d", round(dx));
			displayBigTextLine(4, "dy = %d", round(dy));
			wait1Msec(250);
		}
		#ifdef I2C_Touch
			SetRGB(GREENCOLOR,  GREENCOLOR);
		#endif
		wait1Msec(2000);
		#ifdef I2C_Touch
			SetRGB(BLUECOLOR,  BLUECOLOR);
		#endif
		MoveTo(0, 0);
		delta = (OXAngle - 90) % 360;
		if (abs(delta) > 180)
			delta -= sgn(delta) * 360;
		Rotate(delta);
		break;
	case 3: // code for wall avoidance
		MoveStraight(50, 50);
		while (true){
			curDistance = GetDistance();
			if (abs(curDistance - oldDistance) > 10) eraseDisplay();
			displayCenteredBigTextLine(2, "Distance");
			displayCenteredBigTextLine(4, "%d cm",  curDistance);
			if (curDistance < 30) {
				playSound (soundBeepBeep);
				Rotate(180, 50);
				MoveStraight(50, 50);
			}
			oldDistance = curDistance;
		}
		break;
	case 4: // code for selecting only green and blue balls
		claw = false; waitUntil(!clawState);
		while (true){
			curColor = SensorValue[color];
			if ((curColor == GREENCOLOR) || (curColor == BLUECOLOR)){
				playSound (soundBeepBeep);
				claw = true; waitUntil(clawState);
				wait1Msec(2000);
				claw = false; waitUntil(!clawState);
				wait1Msec(1000);
			}
		}
		break;
	case 5: //code for viewing sonar
		oldDistance = 0;
		while (true){
			curDistance = GetDistance();
			if (curDistance != oldDistance){
				eraseDisplay();
				displayCenteredBigTextLine(0,  "View");
				displayCenteredBigTextLine(2,  "distance");
				displayCenteredBigTextLine(4,  "%d cm",  curDistance);
				oldDistance = curDistance;
			}
			wait1Msec(100);
		}
		break;
	case 6: //code for viewing gyro
		claw = false; waitUntil(!clawState);
		fOldAngle = -100;
		while(true){
			if (gyroAngle != fOldAngle){
				eraseDisplay();
				displayCenteredBigTextLine(0,  "View");
				displayCenteredBigTextLine(2,  "angle");
				displayCenteredBigTextLine(4,  "%.3f",  gyroAngle);
				fOldAngle = gyroAngle;
			}
			wait1Msec(5);
		}
		break;
	case 7: // code for viewing color
		claw = false; waitUntil(!clawState);
		oldColor = GetColor();
		while (true){
			curColor = GetColor();
			if (curColor != oldColor){
				eraseDisplay();
				displayCenteredBigTextLine(0,  "View");
				displayCenteredBigTextLine(2,  "color");
				switch (curColor){
					case  BLACKCOLOR:
						displayCenteredBigTextLine(4,  "Black");
						#ifdef I2C_Touch
							SetRGB(BLACKCOLOR,  BLACKCOLOR);
						#endif
						break;
					case  BLUECOLOR:
						displayCenteredBigTextLine(4,  "Blue");
						#ifdef
							SetRGB(BLUECOLOR,  BLUECOLOR);
						#endif
						playSoundFile("blue.rso");
						break;
					case  GREENCOLOR:
						displayCenteredBigTextLine(4,  "Green");
						#ifdef I2C_Touch
							SetRGB(GREENCOLOR,  GREENCOLOR);
						#endif
						playSoundFile("green.rso");
						break;
					case  YELLOWCOLOR:
						displayCenteredBigTextLine(4,  "Yellow");
						#ifdef I2C_Touch
							SetRGB(YELLOWCOLOR,  YELLOWCOLOR);
						#endif
						playSoundFile("yellow.rso");
						break;
					case  REDCOLOR:
						displayCenteredBigTextLine(4,  "Red");
						#ifdef I2C_Touch
							SetRGB(REDCOLOR,  REDCOLOR);
						#endif
						playSoundFile("red.rso");
						break;
					case  WHITECOLOR:
						displayCenteredBigTextLine(4,  "White");
						#ifdef I2C_Touch
							SetRGB(WHITECOLOR,  WHITECOLOR);
						#endif
						playSoundFile("white.rso");
						break;
					default:
						displayCenteredBigTextLine(4,  "Undefined");
						#ifdef I2C_Touch
							SetRGB(BLACKCOLOR,  BLACKCOLOR);
						#endif
						break;
				}
				oldColor = curColor;
			}
			wait1Msec(100);
		}
		break;
	case 8: // code for playing all sounds
		int nSoundIndex;
		while (true){
			for (nSoundIndex = 0; nSoundIndex <= 113; ++nSoundIndex){
				displayCenteredBigTextLine(3,  "%d",  nSoundIndex);
				playSound((TSounds) nSoundIndex);
				if (bSoundActive){
					while (bSoundActive);
					wait1Msec(500);
				}
			}
		}
		break;
	case 9: // code for viewing pressed button
		nNxtExitClicks  = 3; // Triple clicking EXIT button will terminate program
		eraseDisplay();
		displayCenteredBigTextLine(0, "View");
		displayCenteredBigTextLine(2, "buttons");
		displayCenteredBigTextLine(4, "Nobutton");
		short exit = -1,  count = 0;
		while (true){
			// wait for button press
			while (nNxtButtonPressed == kNoButton);
			curBtn = nNxtButtonPressed;
			if (curBtn != oldBtn){
				eraseDisplay();
				displayCenteredBigTextLine(0, "View");
				displayCenteredBigTextLine(2, "buttons");
				switch (curBtn){
				case kLeftButton:	  displayCenteredBigTextLine(4, "Left"); count++; break;
				case kRightButton:	displayCenteredBigTextLine(4, "Right"); count++; break;
				case kEnterButton:	displayCenteredBigTextLine(4, "Enter"); count ++; break;
				case kExitButton:	  displayCenteredBigTextLine(4, "Exit"); exit++; count = 0; break;
				default:            displayCenteredBigTextLine(4, "Nobutton"); count++; break;
				}
				if (count > 1) exit = -1;
				if (exit == 1) {
					eraseDisplay();
					displayCenteredBigTextLine(0, "FOR EXIT");
					displayCenteredBigTextLine(2, "PRESS");
					displayCenteredBigTextLine(4, "AGAIN");
					wait1Msec(2000);
					exit = -1;
				}
			}
			// wait for button release
			while (nNxtButtonPressed != kNoButton);
		}
		break;
	case 10: // code for master mode
		kOld = 0;  kNew = 0;
		eraseDisplay();
		displayCenteredBigTextLine(0, "WAIT");
		displayCenteredBigTextLine(2, "FOR");
		displayCenteredBigTextLine(4, "CONNECT");
		btDisconnectAll();
		wait1Msec(1000);
		btConnect(3,  "Snatcher");
		while (nBTCurrentStreamIndex == -1);
		wait1Msec(1000);
		btConnect(1,  "Joy1");
		/*
		TFileIOResult status = nBluetoothCmdStatus;
		while (! (status != ioRsltSuccess) && (status != ioRsltCommPending) )
			status = nBluetoothCmdStatus;
		*/
		wait1Msec(5000);
		playSoundFile("link.rso");
		eraseDisplay();
		DrawBTlogo(BT_MASTER_MODE);
		while(true){
			cCmdMessageWriteToBluetooth(1,  bstate,  6,  mailbox1);
			wait1Msec(10);
			cCmdMessageRead(bstate, 5, mailbox2);
			if (bstate[4] && !clawState) {
				claw = true;
				bstate[5] = GetColor();
				cCmdMessageWriteToBluetooth(3, bstate, 5, mailbox3);
			} else
			if (!bstate[4] && clawState) {
				claw = false;
				cCmdMessageWriteToBluetooth(3, bstate, 5, mailbox3);
			}
			if (bstate[1] && !bstate[3] && !bstate[2]) kNew = 1; else
			if (bstate[0] && !bstate[3] && !bstate[2]) kNew = 2; else
			if (bstate[2] && !bstate[0] && !bstate[1]) kNew = 3; else
			if (bstate[3] && !bstate[0] && !bstate[1]) kNew = 4; else
			if (bstate[2] && bstate[0]) kNew = 5; else
			if (bstate[2] && bstate[1]) kNew = 6; else
			if (bstate[3] && bstate[0]) kNew = 7; else
			if (bstate[3] && bstate[1]) kNew = 8; else
			if (!(bstate[0] || bstate[1] || bstate[2] || bstate[3])) kNew = 0;
			if (kOld != kNew){
				cCmdMessageWriteToBluetooth(3, bstate, 5, mailbox3);
				//motor[motorB]=motor[motorC]=0;
				switch (kNew) {
					case 1: Turn(-50); break;
					case 2: Turn(50); break;
					case 3: MoveFwd(MAX_VELOCITY); break;
					case 4: MoveFwd(-MAX_VELOCITY); break;
					case 5: FwdLeft(50); break;
					case 6: FwdRight(50); break;
					case 7: FwdLeft(-50); break;
					case 8: FwdRight(-50); break;
					default: ResetMotor(motorB); ResetMotor(motorC);
				}
				kOld = kNew;
			}
		}
		break;
	case 11: // code for slave mode
		// =====================================================================
		int cmd = 0;
		eraseDisplay();
		displayCenteredBigTextLine(0, "WAIT");
		displayCenteredBigTextLine(2, "FOR");
		displayCenteredBigTextLine(4, "CONNECT");
		waitUntil(nBTCurrentStreamIndex == 0);
		eraseDisplay();
		DrawBTlogo(BT_SLAVE_MODE);
		playSoundFile("link.rso");
		while(true){
			TFileIOResult messageIn = cCmdMessageRead(bstate,  BT_DATA_SIZE,  mailbox1);
			displayTextLine(5, "%d %d %d %d", bstate[0], bstate[1], bstate[2], bstate[3]);
			displayTextLine(7, "%d %d %d %d", bstate[4], bstate[5], bstate[6], bstate[7]);
			// what we must to do with claw?
			if (bstate[4] != old_bstate[4]){
				if (bstate[4] && !clawState) claw = true;
					else if (!bstate[4] && clawState) claw = false;
			}
			// check difference in button's states
			int k = 0;
			for(int i = 0; i < 4; i++)
				if (bstate[i] != old_bstate[i])
					k++;
			if (k > 0){
				cmd = 0;
				if (bstate[1] && !bstate[3] && !bstate[2]) cmd = 1; else
				if (bstate[0] && !bstate[3] && !bstate[2]) cmd = 2; else
				if (bstate[2] && !bstate[0] && !bstate[1]) cmd = 3; else
				if (bstate[3] && !bstate[0] && !bstate[1]) cmd = 4; else
				if (bstate[2]){
					if (bstate[0]) cmd = 5;
						else if (bstate[1]) cmd = 6;
				} else
				if (bstate[3]){
					if (bstate[0]) cmd = 7;
						else if (bstate[1]) cmd = 8;
				}
				if (cmd > 0){
					Stop();
					switch (cmd) {
						case 1: Turn(-bstate[5]);
										#ifdef I2C_Touch
											SetRGB(BLUECOLOR,  BLUECOLOR);
										#endif
										break;
						case 2: Turn(bstate[5]);
										#ifdef I2C_Touch
											SetRGB(BLUECOLOR,  BLUECOLOR);
										#endif
										break;
						case 3: MoveFwd(bstate[5]);
										#ifdef I2C_Touch
											SetRGB(GREENCOLOR,  GREENCOLOR);
										#endif
										break;
						case 4: MoveFwd(-bstate[5]);
										#ifdef I2C_Touch
											SetRGB(GREENCOLOR,  GREENCOLOR);
										#endif
										break;
						case 5: FwdLeft(50);
										#ifdef I2C_Touch
											SetRGB(REDCOLOR,  REDCOLOR);
										#endif
										break;
						case 6: FwdRight(50);
										#ifdef I2C_Touch
											SetRGB(REDCOLOR,  REDCOLOR);
										#endif
										break;
						case 7: FwdLeft(-50);
										#ifdef I2C_Touch
											SetRGB(REDCOLOR,  REDCOLOR);
										#endif
										break;
						case 8: FwdRight(-50);
										#ifdef I2C_Touch
											SetRGB(REDCOLOR,  REDCOLOR);
										#endif
										break;
					}
				}
				else
					Stop();
			}
			displayStringAt(80,20,"%d",cmd);
			for(int i = 0; i < BT_DATA_SIZE; i++)
				old_bstate[i] = bstate[i];
			wait1Msec(50);
		}
		break;
		// =====================================================================
	case 12: //catch everything
		int angle_old = 0, dist_new = 300;
		speed = 20;
		Rotate(-90, speed);
		wait1Msec(300);
		gyroAngle = -90;
		nSyncedMotors = synchBC;
		nSyncedTurnRatio = -100;
		while (gyroAngle < 90){
			motor[motorB] = -speed;
			dist_new = GetDistance();
			OXAngle = 90 - gyroAngle;
			if (dist_new < 70 && dist_new > 10) {
				wait1Msec(300);
				Stop();
				angle_old = gyroAngle;
				OXAngle = 90 - gyroAngle;
				claw = false; waitUntil(!clawState);
				MoveStraight(dist_new - 12);
				claw = true; waitUntil(clawState);
				MoveTo(0,  0);
				claw = false; waitUntil(!clawState);
				wait1Msec(500);
				Rotate(180);
				claw = true; waitUntil(clawState);
				gyroAngle = angle_old;
				nSyncedMotors = synchBC;
				nSyncedTurnRatio = -100;
				wait1Msec(100);
			}
		}
		Stop();
		break;
	case 13: // code for master mode
		kOld = 0;  kNew = 0;
	ubyte channel = 1;
		eraseDisplay();
		if (nBTCurrentStreamIndex >= 0)
			btDisconnectAll();
		while (nBTCurrentStreamIndex != -1) {wait1Msec(5);}
		displayCenteredBigTextLine(0, "CONNECT");
		displayCenteredBigTextLine(2, "TO");
		displayCenteredBigTextLine(4, "JOY");
		btConnect(channel,  "Joy1");
		while (nBTCurrentStreamIndex == -1) {wait1Msec(5);}




		/*
		TFileIOResult status = nBluetoothCmdStatus;
		while (! (status != ioRsltSuccess) && (status != ioRsltCommPending) )
			status = nBluetoothCmdStatus;
		*/
		playSoundFile("link.rso");

		speed = 50;
		while(true){
			cCmdMessageWriteToBluetooth(1,  bstate,  BT_DATA_SIZE,  mailbox1);
			TFileIOResult messageIn = cCmdMessageRead(bstate,  BT_DATA_SIZE,  mailbox2);
			eraseDisplay();
			DrawBTlogo(BT_MASTER_MODE);
			displayTextLine(5, "%d %d %d %d", bstate[0], bstate[1], bstate[2], bstate[3]);
			displayTextLine(7, "%d %d %d %d", bstate[4], bstate[5], bstate[6], bstate[7]);
			// what we must to do with claw?
			if (bstate[4] != old_bstate[4]){
				if (bstate[4] && !clawState) claw = true;
					else if (!bstate[4] && clawState) claw = false;
			}
			// check difference in button's states
			int k = 0;
			for(int i = 0; i < 4; i++)
				if (bstate[i] != old_bstate[i])
					k++;
			if (k > 0){
				cmd = 0;
				if (bstate[1] && !bstate[3] && !bstate[2]) cmd = 1; else
				if (bstate[0] && !bstate[3] && !bstate[2]) cmd = 2; else
				if (bstate[2] && !bstate[0] && !bstate[1]) cmd = 3; else
				if (bstate[3] && !bstate[0] && !bstate[1]) cmd = 4; else
				if (bstate[2]){
					if (bstate[0]) cmd = 5;
						else if (bstate[1]) cmd = 6;
				} else
				if (bstate[3]){
					if (bstate[0]) cmd = 7;
						else if (bstate[1]) cmd = 8;
				}
				if (cmd > 0){
					Stop();
					switch (cmd) {
						case 1: Turn(-bstate[5]);
										#ifdef I2C_Touch
											SetRGB(BLUECOLOR,  BLUECOLOR);
										#endif
										break;
						case 2: Turn(bstate[5]);
										#ifdef I2C_Touch
											SetRGB(BLUECOLOR,  BLUECOLOR);
										#endif
										break;
						case 3: MoveFwd(bstate[5]);
										#ifdef I2C_Touch
											SetRGB(GREENCOLOR,  GREENCOLOR);
										#endif
										break;
						case 4: MoveFwd(-bstate[5]);
										#ifdef I2C_Touch
											SetRGB(GREENCOLOR,  GREENCOLOR);
										#endif
										break;
						case 5: FwdLeft(50);
										#ifdef I2C_Touch
											SetRGB(REDCOLOR,  REDCOLOR);
										#endif
										break;
						case 6: FwdRight(50);
										#ifdef I2C_Touch
											SetRGB(REDCOLOR,  REDCOLOR);
										#endif
										break;
						case 7: FwdLeft(-50);
										#ifdef I2C_Touch
											SetRGB(REDCOLOR,  REDCOLOR);
										#endif
										break;
						case 8: FwdRight(-50);
										#ifdef I2C_Touch
											SetRGB(REDCOLOR,  REDCOLOR);
										#endif
										break;
					}
				}
				else
					Stop();
			}
			displayStringAt(80,20,"%d",cmd);
			for(int i = 0; i < BT_DATA_SIZE; i++)
				old_bstate[i] = bstate[i];
			wait1Msec(50);

		/*
		cCmdMessageWriteToBluetooth(1,  bstate,  BT_DATA_SIZE,  mailbox1);
			//wait1Msec(10);
			TFileIOResult messageIn = cCmdMessageRead(bstate,  BT_DATA_SIZE,  mailbox2);
			eraseDisplay();
			DrawBTlogo(BT_MASTER_MODE)
			displayTextLine(5, "%d %d %d %d", bstate[0], bstate[1], bstate[2], bstate[3]);
			displayTextLine(7, "%d %d %d %d", bstate[4], bstate[5], bstate[6], bstate[7]);

			if (bstate[4] && !clawState) {
				claw = true;
				bstate[5] = GetColor();
				cCmdMessageWriteToBluetooth(1, bstate, BT_DATA_SIZE, mailbox1);
			} else
			if (!bstate[4] && clawState) {
				claw = false;
				cCmdMessageWriteToBluetooth(1, bstate, BT_DATA_SIZE, mailbox1);
			}
			if (bstate[1] && !bstate[3] && !bstate[2]) kNew = 1; else
			if (bstate[0] && !bstate[3] && !bstate[2]) kNew = 2; else
			if (bstate[2] && !bstate[0] && !bstate[1]) kNew = 3; else
			if (bstate[3] && !bstate[0] && !bstate[1]) kNew = 4; else
			if (bstate[2] && bstate[0]) kNew = 5; else
			if (bstate[2] && bstate[1]) kNew = 6; else
			if (bstate[3] && bstate[0]) kNew = 7; else
			if (bstate[3] && bstate[1]) kNew = 8; else
			if (!(bstate[0] || bstate[1] || bstate[2] || bstate[3])) kNew = 0;
			if (kOld != kNew){
				speed = 50;
				cCmdMessageWriteToBluetooth(1, bstate, BT_DATA_SIZE, mailbox1);
				//motor[motorB]=motor[motorC]=0;
				switch (kNew) {
					case 1: Turn(-50); break;
					case 2: Turn(50); break;
					case 3: MoveFwd(speed); break;
					case 4: MoveFwd(-speed); break;
					case 5: FwdLeft(50); break;
					case 6: FwdRight(50); break;
					case 7: FwdLeft(-50); break;
					case 8: FwdRight(-50); break;
					default: Stop();
				}
				kOld = kNew;
			} else {
				if (kNew == 3 || kNew == 4) {
					speed = speed + 5;
					if (speed > 100)
						speed = 100;
					if (kNew == 3)
						MoveFwd(speed);
							else
								MoveFwd(-speed);
				}
			}

		for(int i = 0; i < BT_DATA_SIZE; i++)
				old_bstate[i] = bstate[i];
			wait1Msec(50);
	   */
		}
		break;
	default:
		break;
	}
}
