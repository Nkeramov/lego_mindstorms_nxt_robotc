#pragma config(Sensor, S1, touch, sensorTouch)
#pragma config(Sensor, S2, gyro, sensorI2CHiTechnicGyro)
#pragma config(Sensor, S3, color, sensorColorNxtFULL)
#pragma config(Sensor, S4, sonar, sensorSONAR)

#include "utils.c"
#include "bluetooth.c"
#include "sensors.c"


//#define MindSensors_IMU		// #pragma config(Sensor, S2, gyro, sensorI2CCustomFastSkipStates)
#define HiTechnic_Gyro 			// #pragma config(Sensor, S2, gyro, sensorI2CHiTechnicGyro)

#define I2C_Touch						// #pragma config(Sensor, S1, touch, sensorI2CCustom)
//#define LEGO_Touch				// #pragma config(Sensor, S1, touch, sensorTouch)

#ifdef HiTechnic_Gyro
		#include "hitechnic_gyro_driver.c"
#endif
#ifdef MindSensors_IMU
		#include "mindsensors_imu.h"
		#include "mindsensors_gyro_driver.c"
#endif

#define MOVE_POINTS										5
#define CLAW_CLOSE_SPEED 							40
#define CLAW_OPEN_SPEED 							40
#define CLAW_HOLD_SPEED 							15
#define CLAW_OPEN_ENCODER 						600

#define FRICTION_VAL									0.02899
//0.037 - wheels
//0.029 - caterpillars
//0.02842	printed
//0.03944 new

enum TJoystickCommand {
  CMD_NONE = 0,
  CMD_TURN_LEFT,
  CMD_TURN_RIGHT,
  CMD_FWD,
  CMD_BACK,
  CMD_FWD_LEFT,
  CMD_FWD_RIGHT,
  CMD_BACK_LEFT,
  CMD_BACK_RIGHT
};

string programNames[14][2] = {
  {"TEST MOTORS", "AND CLAW"},
  {"FIND AND CATCH", "NEAREST OBJECT"},
  {"RANDOM MOVING", "AND RETURNING"},
  {"WALL", "AVOIDANCE"},
  {"FIND AND CATCH", "EVERYTHING"},
  {"SELECT GREEN", "AND BLUE COLORS"},
  {"VIEW SONAR", "SENSOR"},
  {"VIEW GYRO", "SENSOR"},
  {"VIEW COLOR", "SENSOR"},
  {"PLAY ALL", "SOUNDS"},
  {"VIEW PRESSED", "BUTTON"},
  {"BT CONTROL", "SLAVE"},
  {"BT CONTROL", "SINGLE MASTER"},
  {"BT CONTROL", "DUAL MASTER"}
};

TNxtButtons curBtn = kNoButton, oldBtn = kNoButton;
bool newClawState = true, curClawState = false;
int azimuth = 90;
float curX = 0, curY = 0;


#ifdef I2C_Touch
	void setRGB(TColors cleft,  TColors cright){
		ubyte i2cconfg[3];
		const tSensors kPort = S1;
		const short kSize = 0;
		const short kAddress = 1;
		const short kData = 2;
		typedef ubyte kResults[4];
		ubyte nAddress = 0x40;
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
		while (nI2CStatus[kPort] == STAT_COMM_PENDING){
		  wait1Msec(100);
		}
	}
#endif

bool getTouch(){
  bool state = false;
  #ifdef I2C_Touch
		SensorType[touch] = sensorTouch;
    wait1Msec(50);
    state = SensorValue[touch];
  	SensorType[S1] = sensorI2CCustom;
  #elif defined(LEGO_Touch)
  	state = SensorValue[touch];
  #endif
  return state;
}

void resetMotor(tMotor mname){
	if (mname != motorA)
		nSyncedMotors = synchNone;
	motor[mname] = 0;
	nMotorEncoder[mname] = 0;
	nMotorEncoderTarget[mname] = 0;
}

void stopMotors(){
	resetMotor(motorB);
	resetMotor(motorC);
}

void moveFwd(short speed = 30){
  nSyncedMotors = synchBC;
  nSyncedTurnRatio = 100;
	motor[motorB] = speed;
}

void turn(short speed = 50){
	nSyncedMotors = synchBC;
	nSyncedTurnRatio = -100;
	motor[motorB] = speed;
}

void fwdLeft(short speed = 50){
	nSyncedMotors = synchBC;
	nSyncedTurnRatio = 75;
	motor[motorB] = speed;
}

void fwdRight(short speed = 50){
	nSyncedMotors = synchCB;
	nSyncedTurnRatio = 75;
	motor[motorC] = speed;
}

void moveStraight(int dist, short speed = 40){
	if (dist < 0){
		speed = -abs(speed);
		dist = abs(dist);
	}
	stopMotors();
	float oldGyroAngle = gyroAngle;
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
		float e = gyroAngle - oldGyroAngle;
		//oldGyroAngle = gyroAngle;
		if (fabs(e) >= 2.0){
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
		wait1Msec(50);
	}
	//eraseDisplay();
	//displayBigTextLine(0, "%.2f",old_gyroAngle);
	//displayBigTextLine(4, "%.2f",gyroAngle);
	stopMotors();
	curX += sgn(speed) * dist * cosDegrees(azimuth);
	curY += sgn(speed) * dist * sinDegrees(azimuth);
}

void rotate(float angle = 180, short speed = 30){
	stopMotors();
	azimuth -= angle;
	speed = abs(speed);
	if (angle > 0)
		speed *= -1;
	angle = fabs(angle);
	gyroAngle = 0;
	turn(speed);
	/*
	if (angle > 30) {
		waitUntil(abs(gyroAngle) >= angle - 30);
		motor[motorB] = round(speed * 2.0 / 3.0);
	}
	else motor[motorB] = round(speed * 2.0 / 3.0);
	*/
	waitUntil(fabs(gyroAngle) > angle);
	stopMotors();
}

task Claw {
	while (true){
		if (newClawState && !curClawState) {
			resetMotor(motorA);
			motor[motorA] = CLAW_CLOSE_SPEED;
			waitUntil(getTouch());
			resetMotor(motorA);
			motor[motorA] = CLAW_HOLD_SPEED;
			curClawState = true;
		} else
		if (!newClawState && curClawState){
			resetMotor(motorA);
			motor[motorA] = CLAW_CLOSE_SPEED ;
			waitUntil(getTouch());
			resetMotor(motorA);
			motor[motorA] = -CLAW_OPEN_SPEED ;
			waitUntil(abs(nMotorEncoder[motorA]) >= CLAW_OPEN_ENCODER);
			resetMotor(motorA);
			curClawState = false;
		}
		wait1Msec(5);
	}
}

TJoystickCommand parseButtons(ubyte *btData){
    if (btData[1] && !btData[3] && !btData[2]) return CMD_TURN_LEFT;
    if (btData[0] && !btData[3] && !btData[2]) return CMD_TURN_RIGHT;
    if (btData[2] && !btData[0] && !btData[1]) return CMD_FWD;
    if (btData[3] && !btData[0] && !btData[1]) return CMD_BACK;
    if (btData[2] && btData[0]) return CMD_FWD_LEFT;
    if (btData[2] && btData[1]) return CMD_FWD_RIGHT;
    if (btData[3] && btData[0]) return CMD_BACK_LEFT;
    if (btData[3] && btData[1]) return CMD_BACK_RIGHT;
    return CMD_NONE;
}

void executeCommand(TJoystickCommand cmd, ubyte speed = 50){
	stopMotors();
	switch (cmd) {
		case CMD_TURN_LEFT: turn(-speed); break;
		case CMD_TURN_RIGHT: turn(speed); break;
		case CMD_FWD: moveFwd(speed); break;
		case CMD_BACK: moveFwd(-speed); break;
		case CMD_FWD_LEFT: fwdLeft(speed); break;
		case CMD_FWD_RIGHT: fwdRight(speed); break;
		case CMD_BACK_LEFT: fwdLeft(-speed); break;
		case CMD_BACK_RIGHT: fwdRight(-speed); break;
		default: break;
	}
	#ifdef I2C_Touch
		if (cmd == CMD_TURN_LEFT || cmd == CMD_TURN_RIGHT)
			setRGB(BLUECOLOR,  BLUECOLOR);
		else if (cmd == CMD_FWD || cmd == CMD_BACK)
			setRGB(GREENCOLOR,  GREENCOLOR);
		else if (cmd == CMD_FWD_LEFT || cmd == CMD_FWD_RIGHT || cmd == CMD_BACK_LEFT || cmd == CMD_BACK_RIGHT)
			setRGB(REDCOLOR,  REDCOLOR);
	#endif
}

void processBluetoothPacket(ubyte *btData, ubyte *btDataOld){
	// what we must to do with newClawState?
	if (btData[4] != btDataOld[4]){
		if (btData[4] && !curClawState) newClawState = true;
		else
			if (!btData[4] && curClawState) newClawState = false;
	}
	TJoystickCommand cmd = parseButtons(btData);
	ubyte speed = btData[5];
	executeCommand(cmd, speed);
}

void searchAngle(int &minAngle, int &minDist, int maxAngle = 90, short speed = 10){
	maxAngle = abs(maxAngle) % 360;
	speed = abs(speed);
	rotate(-maxAngle, speed);
	wait1Msec(300);
	gyroAngle = -maxAngle;
	minDist = SONAR_MAX_DISTANCE;
	turn(-speed);
	while (gyroAngle < maxAngle){
		int newDist = getDistance();
		if (newDist < minDist) {
			minDist = newDist;
			minAngle = ceil(gyroAngle);
		}
		wait1Msec(5);
	}
	stopMotors();
	wait1Msec(300);
	rotate(ceil(-gyroAngle),  speed);
	//distance from newClawState (color sensor) to backward of sonar
	minDist = minDist - 10;
}

//r+  l-  OX Reversed
void moveTo(float x,  float y){
	float dx = curX - x,  dy = curY - y;
	int length = sqrt(dx * dx + dy * dy);
	int delta = azimuth;
	if (dx < 0)
			delta -= radiansToDegrees(atan(dy / dx));
		else if (dx > 0)
			delta -= radiansToDegrees(atan(dy / dx)) + 180;
			else
				delta += sgn(dy) * 90;
	delta = delta % 360;
	if (abs(delta) > 180)
		delta -= sgn(delta) * 360;
	rotate(delta);
	wait1Msec(300);
	moveStraight(length,  80);
}

void testSnatcher(){
	eraseDisplay();
	displayCenteredBigTextLine(2, "TESTING");
	moveStraight(15, 50);
	wait1Msec(1000);
	moveStraight(-15, 50);
	wait1Msec(1000);
	rotate(180, 30);
	wait1Msec(1000);
	rotate(-180, 30);
	wait1Msec(1000);
	newClawState = true;
	waitUntil(getTouch());
	wait1Msec(1000);
	newClawState = false;
	playSound(soundBeepBeep);
	newClawState = true;
	waitUntil(getTouch());
	rotate(45, 30);
	wait1Msec(1000);
	rotate(-90, 30);
	wait1Msec(1000);
	rotate(45, 30);
	playSound (soundBeepBeep);
	#ifdef I2C_Touch
		setRGB(REDCOLOR,  REDCOLOR);
		wait1Msec(500);
		setRGB(GREENCOLOR,  GREENCOLOR);
		wait1Msec(500);
		setRGB(BLUECOLOR,  BLUECOLOR);
		wait1Msec(500);
		setRGB(BLACKCOLOR,  BLACKCOLOR);
	#endif
	newClawState = false;
}

void printSelectMenu(short curProg = 0){
	eraseDisplay();
	displayCenteredBigTextLine(0, "SELECT");
	displayCenteredBigTextLine(2, "PROGRAM");
	displayCenteredBigTextLine(4, "%d", curProg);
  displayCenteredTextLine(6, "%s", programNames[curProg][0]);
	displayCenteredTextLine(7, "%s", programNames[curProg][1]);
}

void printLogo(){
	eraseDisplay();
	displayCenteredBigTextLine(0, "LEGO");
	displayCenteredBigTextLine(2, "SNATCHER");
}

short selectProgram(void){
	short curProg = 0, selProg = -1;
	bool exit = false;
	nNxtExitClicks  = 2;
	printSelectMenu(curProg);
	while (selProg < 0){
		curBtn = waitForButtonPress();
		playSound(soundBlip);
		switch (curBtn){
			case kLeftButton: curProg--; if (curProg < 0) curProg = 13;	break;
			case kRightButton: curProg++; if (curProg > 13) curProg = 0;	break;
			case kEnterButton: selProg = curProg; break;
			case kExitButton: exit = true;  break;
		}
		if (exit){
			eraseDisplay();
			displayCenteredBigTextLine(0, "FOR EXIT");
			displayCenteredBigTextLine(2, "PRESS");
			displayCenteredBigTextLine(4, "AGAIN");
			wait1Msec(1000);
			exit = false;
		}
		printSelectMenu(curProg);
		waitForButtonRelease();
	}
	// program selected go to perform it
	eraseDisplay();
	displayCenteredBigTextLine(2, "SELECTED");
	displayCenteredBigTextLine(4, "%d", selProg);
	wait1Msec(500);
	return selProg;
}

void initSnatcher(){
	startTask(Claw);
	wait1Msec(1000);
	newClawState = true; waitUntil(curClawState);
	printLogo();
	displayCenteredTextLine(6, "INIT");
	displayCenteredTextLine(7, "BLUETOOTH");
	initBluetooth();
	playSound (soundBeepBeep);
	printLogo();
	displayCenteredTextLine(6, "INIT");
	displayCenteredTextLine(7, "GYROSCOPE");
	waitUntil(gyroState == stateGyroWorking);
	playSound(soundBeepBeep);
}

task main(){
	initSnatcher();
	int numberProgram = selectProgram();
	int speed = 0;
	switch (numberProgram){
	case 0: // self-test code
		bool test = true;
		while(test == true){
			testSnatcher();
			eraseDisplay();
			displayCenteredBigTextLine(0, "REPEAT");
			displayCenteredBigTextLine(2, "TEST?");
			displayCenteredTextLine(6, "PRESS");
			displayCenteredTextLine(7, "ENTER/EXIT");
			curBtn = waitForButtonPress();
			if (curBtn == kExitButton)
				test = false;
		}
		break;
	case 1: // searching objects
		int angle = 0, distance = 0;
		searchAngle(angle,  distance,  90,  20);
		eraseDisplay();
		displayBigTextLine(0, "a = %d", angle);
		displayBigTextLine(2, "d = %d", distance);
		rotate(angle);
		newClawState = false; waitUntil(!curClawState);
		moveStraight(distance, 50);
		newClawState = true; waitUntil(curClawState);
		switch (getColor()){
			case  BLACKCOLOR: displayBigTextLine(4, "Black"); playSoundFile("black.rso"); break;
			case  BLUECOLOR:	displayBigTextLine(4, "Blue"); playSoundFile("blue.rso"); break;
			case  GREENCOLOR:	displayBigTextLine(4, "Green"); playSoundFile("green.rso"); break;
			case  YELLOWCOLOR: displayBigTextLine(4, "Yellow"); playSoundFile("yellow.rso"); break;
			case  REDCOLOR: displayBigTextLine(4, "Red"); playSoundFile("red.rso"); break;
			case  WHITECOLOR: displayBigTextLine(4, "White"); playSoundFile("white.rso"); break;
			default: displayBigTextLine(4, "Undefined"); break;
		}
		break;
	case 2: // moving and returning
		#ifdef I2C_Touch
			setRGB(REDCOLOR,  REDCOLOR);
		#endif
		float dx = 0, dy = 0;
		int dist, delta = 0, old_delta = 0;
		for(int i = 0; i < MOVE_POINTS; i++){
			old_delta = delta;
			while (delta == old_delta)
				delta = (abs(rand() % 5) - 2) * 45;  // from -90 to 90
			rotate(delta);
			wait1Msec(250);
			dist = 512;
			// 10 - offset to obstacle
			while (dist > getDistance() - 10)
				dist = (abs(rand() % 3) + 3) * 10;
			moveStraight(dist);
			dx += cosDegrees(azimuth) * dist;
			dy += sinDegrees(azimuth) * dist;
			eraseDisplay();
			displayBigTextLine(0, "a = %d", azimuth);
			displayBigTextLine(2, "dx = %d", round(dx));
			displayBigTextLine(4, "dy = %d", round(dy));
			wait1Msec(250);
		}
		#ifdef I2C_Touch
			setRGB(GREENCOLOR,  GREENCOLOR);
		#endif
		wait1Msec(2000);
		#ifdef I2C_Touch
			setRGB(BLUECOLOR,  BLUECOLOR);
		#endif
		moveTo(0, 0);
		delta = (azimuth - 90) % 360;
		if (abs(delta) > 180)
			delta -= sgn(delta) * 360;
		rotate(delta);
		break;
	case 3: // code for wall avoidance
		moveStraight(50, 50);
		while (true){
			curDistance = getDistance();
			if (abs(curDistance - oldDistance) > 10)
				eraseDisplay();
			displayCenteredBigTextLine(2, "Distance");
			displayCenteredBigTextLine(4, "%d cm",  curDistance);
			if (curDistance < 30) {
				playSound (soundBeepBeep);
				rotate(180, 50);
				moveStraight(50, 50);
			}
			oldDistance = curDistance;
		}
		break;
	case 4: //catch everything
		int angle_old = 0, dist_new = 300;
		speed = 20;
		rotate(-90, speed);
		wait1Msec(300);
		gyroAngle = -90;
		nSyncedMotors = synchBC;
		nSyncedTurnRatio = -100;
		while (gyroAngle < 90){
			motor[motorB] = -speed;
			dist_new = getDistance();
			azimuth = 90 - gyroAngle;
			if (dist_new < 70 && dist_new > 10) {
				wait1Msec(300);
				stopMotors();
				angle_old = gyroAngle;
				azimuth = 90 - gyroAngle;
				newClawState = false; waitUntil(!curClawState);
				moveStraight(dist_new - 12);
				newClawState = true; waitUntil(curClawState);
				moveTo(0,  0);
				newClawState = false; waitUntil(!curClawState);
				wait1Msec(500);
				rotate(180);
				newClawState = true; waitUntil(curClawState);
				gyroAngle = angle_old;
				nSyncedMotors = synchBC;
				nSyncedTurnRatio = -100;
				wait1Msec(100);
			}
		}
		stopMotors();
		break;
	case 5: // code for selecting only green and blue balls
		newClawState = false; waitUntil(!curClawState);
		while (true){
			curColor = SensorValue[color];
			if ((curColor == GREENCOLOR) || (curColor == BLUECOLOR)){
				playSound (soundBeepBeep);
				newClawState = true; waitUntil(curClawState);
				wait1Msec(2000);
				newClawState = false; waitUntil(!curClawState);
				wait1Msec(1000);
			}
		}
		break;
	case 6: // code for viewing sonar
		oldDistance = 0;
		while (true){
			curDistance = getDistance();
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
	case 7: //code for viewing gyro
		//newClawState = false; waitUntil(!curClawState);
		gyroAngle = 0;
		float oldGyroAngle = gyroAngle - 100;
		while(true){
			if (gyroAngle != oldGyroAngle){
				eraseDisplay();
				displayCenteredBigTextLine(0,  "View");
				displayCenteredBigTextLine(2,  "angle");
				displayCenteredBigTextLine(4,  "%.3f",  gyroAngle);
				oldGyroAngle = gyroAngle;
			}
			wait1Msec(100);
		}
		break;
	case 8: // code for viewing color
		newClawState = false; waitUntil(!curClawState);
		oldColor = getColor();
		while (true){
			curColor = getColor();
			if (curColor != oldColor){
				eraseDisplay();
				displayCenteredBigTextLine(0,  "View");
				displayCenteredBigTextLine(2,  "color");
				switch (curColor){
					case  BLACKCOLOR: displayCenteredBigTextLine(4, "Black"); playSoundFile("black.rso"); break;
					case  BLUECOLOR:	displayCenteredBigTextLine(4, "Blue"); playSoundFile("blue.rso"); break;
					case  GREENCOLOR:	displayCenteredBigTextLine(4, "Green"); playSoundFile("green.rso"); break;
					case  YELLOWCOLOR: displayCenteredBigTextLine(4, "Yellow"); playSoundFile("yellow.rso"); break;
					case  REDCOLOR: displayCenteredBigTextLine(4, "Red"); playSoundFile("red.rso"); break;
					case  WHITECOLOR: displayCenteredBigTextLine(4, "White"); playSoundFile("white.rso"); break;
					default: displayCenteredBigTextLine(4, "Undefined"); break;
				}
				displayCenteredBigTextLine(6, "int = %d", curColor);
				#ifdef I2C_Touch
					setRGB(curColor,  curColor);
				#endif
				oldColor = curColor;
			}
			wait1Msec(100);
		}
		break;
	case 9: // code for playing all sounds
		while (true){
			for (int nSoundIndex = 0; nSoundIndex <= 113; ++nSoundIndex){
				displayCenteredBigTextLine(3,  "%d",  nSoundIndex);
				playSound((TSounds) nSoundIndex);
				if (bSoundActive){
					while (bSoundActive){
						wait1Msec(100);
					}
					wait1Msec(500);
				}
			}
		}
		break;
	case 10: // code for viewing pressed button
		// triple clicking EXIT button will terminate program
		nNxtExitClicks = 3;
		eraseDisplay();
		displayCenteredBigTextLine(0, "View");
		displayCenteredBigTextLine(2, "buttons");
		displayCenteredBigTextLine(4, "No button");
		short exit = -1, count = 0;
		while (true){
			curBtn = waitForButtonPress();
			if (curBtn != oldBtn){
				eraseDisplay();
				displayCenteredBigTextLine(0, "View");
				displayCenteredBigTextLine(2, "buttons");
				switch (curBtn){
				case kLeftButton:	  displayCenteredBigTextLine(4, "Left"); count++; break;
				case kRightButton:	displayCenteredBigTextLine(4, "Right"); count++; break;
				case kEnterButton:	displayCenteredBigTextLine(4, "Enter"); count ++; break;
				case kExitButton:	  displayCenteredBigTextLine(4, "Exit"); exit++; count = 0; break;
				default:            displayCenteredBigTextLine(4, "No button"); count++; break;
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
			waitForButtonRelease();
		}
		break;
	case 11: // code for BT control slave mode
		eraseDisplay();
		displayCenteredBigTextLine(0, "WAITING");
		displayCenteredBigTextLine(2, "FOR");
		displayCenteredBigTextLine(4, "CONNECT");
		displayCenteredTextLine(7, "SLAVE");
		initBluetooth();
	  waitBluetoothConnected();
		playSoundFile("link.rso");
    drawInterface(BT_MODE_SLAVE);
		while(true){
			TFileIOResult nBTCmdRdErrorStatus = cCmdMessageRead(btData,  BT_DATA_SIZE,  mailbox1);
			if (nBTCmdRdErrorStatus == ioRsltSuccess){
				// check difference in button's states
				if (memcmp(btData, btDataOld, BT_DATA_SIZE) != 0) {
					drawInterface(BT_MODE_SLAVE);
				  processBluetoothPacket(btData, btDataOld);
					memcpy(btDataOld, btData, BT_DATA_SIZE);
				}
			}
			wait1Msec(50);
		}
		break;
	case 12: // code for BT control single master mode
	  ubyte channel = 1;
		eraseDisplay();
		displayCenteredBigTextLine(0, "WAITING");
		displayCenteredBigTextLine(2, "FOR");
		displayCenteredBigTextLine(4, "CONNECT");
		displayCenteredTextLine(7, "SINGLE MASTER");
		initBluetooth();
		btConnect(channel,  "Joy");
	  waitBluetoothConnected();
    playSoundFile("link.rso");
    drawInterface(BT_MODE_MASTER, curClawState, speed, getDistance(), (ubyte)getColor());
    speed = 50;
		while(true){
			ubyte btMasterData[BT_DATA_SIZE];
			memset(btMasterData, 0, BT_DATA_SIZE);
			btMasterData[4] = curClawState;
    	btMasterData[5] = speed;
    	//btMasterData[6] = getDistance();
    	btMasterData[6] = curDistance;
    	//btMasterData[7] = getColor();
			btMasterData[7] = (ubyte)curColor;
    	cCmdMessageWriteToBluetooth(1, btMasterData, BT_DATA_SIZE, mailbox1);
			TFileIOResult nBTCmdRdErrorStatus = cCmdMessageRead(btData, BT_DATA_SIZE, mailbox2);
			if (nBTCmdRdErrorStatus == ioRsltSuccess){
				// check difference in button's states
				if (memcmp(btData, btDataOld, BT_DATA_SIZE) != 0) {
					speed = btData[5];
					processBluetoothPacket(btData, btDataOld);
					memcpy(btDataOld, btData, BT_DATA_SIZE);
				}
			}
			//drawInterface(BT_MODE_MASTER, curClawState, speed, getDistance(), (ubyte)getColor());
			drawInterface(BT_MODE_MASTER, curClawState, speed, curDistance, (ubyte)curColor);
			wait1Msec(50);
		}
		break;
	case 13: // code for BT control dual master mode
		eraseDisplay();
		displayCenteredBigTextLine(0, "WAITING");
		displayCenteredBigTextLine(2, "FOR");
		displayCenteredBigTextLine(4, "CONNECT");
		displayCenteredTextLine(7, "DUAL MASTER");
		initBluetooth();
		btConnect(3,  "Snatcher");
	  waitBluetoothConnected();
		btConnect(1,  "Joy");
		wait1Msec(5000);
		playSoundFile("link.rso");
		drawInterface(BT_MODE_MASTER);
		while(true){
			cCmdMessageWriteToBluetooth(1,  btData,  6,  mailbox1);
			wait1Msec(10);
			cCmdMessageRead(btData, 5, mailbox2);
			if (btData[4] && !curClawState) {
				newClawState = true;
				btData[5] = getColor();
				cCmdMessageWriteToBluetooth(3, btData, 5, mailbox3);
			} else
			if (!btData[4] && curClawState) {
				newClawState = false;
				cCmdMessageWriteToBluetooth(3, btData, 5, mailbox3);
			}
			TJoystickCommand cmd = parseButtons(btData);
			if (cmd != CMD_NONE){
				cCmdMessageWriteToBluetooth(3, btData, 5, mailbox3);
				//ubyte speed = btData[5];
				executeCommand(cmd, speed);
			}
		}
		break;
	}
}
