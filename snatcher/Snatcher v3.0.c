#pragma config(Sensor, S1,     touch,          sensorTouch)
#pragma config(Sensor, S2,     gyro,           sensorI2CHiTechnicGyro)
#pragma config(Sensor, S3,     color,          sensorColorNxtFULL)
#pragma config(Sensor, S4,     sonar,          sensorSONAR)

const int sonar_average_count = 10;
const int move_points = 5;

const float kMove = 0.029;
//0.037 - wheels
//0.029 - caterpillars

#include "HiTechnic_new_gyroDriver.c"

TNxtButtons new_btn = kNoButton;
TNxtButtons old_btn = kNoButton;
TColors new_color = BLACKCOLOR;
TColors old_color = BLACKCOLOR;
int new_dest = 0;
int old_dest = 0;


void ResetMotor(tMotor mname){
	motor[mname] = 0;
	nMotorEncoder[mname] = 0;
	nMotorEncoderTarget[mname] = 0;
	nSyncedMotors = synchNone;
}

void MoveStraight(int dist,int speed=40){
	if (dist*speed<0) speed = -abs(speed);
	dist = abs(dist);
	ResetMotor(motorB);
	ResetMotor(motorC);
	nSyncedMotors = synchBC ;
	nSyncedTurnRatio = 100;
	motor[motorB] = speed;
	while(abs(nMotorEncoder[motorB])*kMove<dist){
		//displayBigStringAt(0, 40, "%f ", abs(nMotorEncoder[motorB]));
	}
	ResetMotor(motorB);
	ResetMotor(motorC);
}

void ClawUp(int speed=40){
	ResetMotor(motorA);
	motor[motorA] = speed;
	while(SensorValue[touch] == 0){}
	//ResetMotor(motorA);
	//wait1Msec(500);
	motor[motorA] = speed / 2;
}

void ClawDown(int speed=30){
	//ClawUp();
	ResetMotor(motorA);
	speed *=-1;
	motor[motorA] = speed;
	while (abs(nMotorEncoder[motorA]) < 400){}
	ResetMotor(motorA);
}

void Rotate(float angle = 180, int speed=30){
	ResetMotor(motorB);
	ResetMotor(motorC);
	speed=abs(speed);
	if(angle>0) speed*=-1;
	angle = abs(angle);
	gyroAngle = 0;
	nSyncedMotors = synchBC;
	nSyncedTurnRatio = -100;
	motor[motorB] = speed;
	while (abs(gyroAngle) < angle){}
	ResetMotor(motorB);
	ResetMotor(motorC);
}

int GetDistance(int average_count){
	int count = 0;
	int sum = 0;
	int dist = 0;
	while (count < average_count){
		dist = SensorValue[sonar];
		if (((dist != 255) || ((sum / (count+1)) >= 100) || (sum == 0))){
			count++;
			sum += dist;
		}
		wait1Msec(200);
	}
	return ceil(sum / average_count);
}

void SearchAngle(int *ang, int *dist,int angle_max = 90, int speed = 30){
	angle_max = abs(angle_max);
	speed = abs(speed);
	int angle_min = 0;
	int dist_new = 0;
	int dist_min = 255;
	int dist_old = 255;
	int angle_old = 0;
	Rotate(-angle_max,speed);
	wait1Msec(300);
	gyroAngle = -angle_max;
	nSyncedMotors = synchBC;
	nSyncedTurnRatio = -100;
	motor[motorB] = -speed;
	while(gyroAngle<angle_max){
		dist_new = SensorValue[sonar];
		if (dist_new < dist_min) {
			dist_min = dist_new;
			angle_min = ceil((gyroAngle + angle_old));
		}
	}
	ResetMotor(motorB);
	ResetMotor(motorC);
	wait1Msec(300);
	Rotate(ceil(-gyroAngle), speed);
	*ang = angle_min;
	*dist = dist_min;
}

void TestSnatcher(){
	eraseDisplay();
	displayBigStringAt(14, 55, "TESTING");
	MoveStraight(15,50);
	wait1Msec(1000);
	MoveStraight(-15,50);
	wait1Msec(1000);
	Rotate(180,30);
	wait1Msec(1000);
	Rotate(-180,30);
	wait1Msec(1000);
	ClawUp();
	wait1Msec(1000);
	ClawDown();
	playSound(soundBeepBeep);
	ClawUp();
	Rotate(45,30);
	wait1Msec(1000);
	Rotate(-90,30);
	wait1Msec(1000);
	Rotate(45,30);
	ClawDown();
}

int SelectProgram(void){
	int selProg = -1;
	int curProg = 0;
	bool exit = false;
	// double clicking EXIT button will terminate program
	nNxtExitClicks  = 2;
	eraseDisplay();
	displayCenteredBigTextLine(0,"SELECT");
	displayCenteredBigTextLine(2,"PROGRAM");
	displayCenteredBigTextLine(4,"%d",curProg);
	displayCenteredTextLine(6,"test motors");
	displayCenteredTextLine(7,"and claw");
	// selecting program
	while (selProg < 0){
		// wait for button press
		while (nNxtButtonPressed == kNoButton){}
		new_btn = nNxtButtonPressed;
		playSound(soundBlip);
		switch (new_btn){
			case kLeftButton: curProg--; if (curProg < 0) curProg = 9;	break;
			case kRightButton:	curProg++; if (curProg > 9) curProg = 0;	break;
			case kEnterButton:	selProg = curProg; break;
			case kExitButton:	 exit = true;  break;
		}
		if (exit == true){
			eraseDisplay();
			displayCenteredBigTextLine(0,"FOR EXIT");
			displayCenteredBigTextLine(2,"PRESS");
			displayCenteredBigTextLine(4,"AGAIN");
			wait1Msec(1000);
			exit = false;
		}
		eraseDisplay();
		displayCenteredBigTextLine(0,"SELECT");
		displayCenteredBigTextLine(2,"PROGRAM");
		displayCenteredBigTextLine(4,"%d",curProg);
		switch (curProg){
		case 0:
			displayCenteredTextLine(6,"test motors");
			displayCenteredTextLine(7,"and claw");
			break;
		case 1:
			displayCenteredTextLine(6,"find and catch");
			displayCenteredTextLine(7,"nearest object");
			break;
		case 2:
			displayCenteredTextLine(6,"random moving");
			displayCenteredTextLine(7,"and returning");
			break;
		case 3:
			displayCenteredTextLine(6,"wall");
			displayCenteredTextLine(7,"avoidance");
			break;
		case 4:
			displayCenteredTextLine(6,"select green");
			displayCenteredTextLine(7,"and blue colors");
			break;
		case 5:
			displayCenteredTextLine(6,"view sonar");
			displayCenteredTextLine(7,"sensor");
			break;
		case 6:
			displayCenteredTextLine(6,"view gyro");
			displayCenteredTextLine(7,"sensor");
			break;
		case 7:
			displayCenteredTextLine(6,"view color");
			displayCenteredTextLine(7,"sensor");
			break;
		case 8:
			displayCenteredTextLine(6,"play all");
			displayCenteredTextLine(7,"sounds");
			break;
		case 9:
			displayCenteredTextLine(6,"view pressed");
			displayCenteredTextLine(7,"button");
			break;
		}
		// wait for button release
		while (nNxtButtonPressed != kNoButton){}
	}
	// program selected go to perform it
	eraseDisplay();
	displayCenteredBigTextLine(2,"SELECTED");
	displayCenteredBigTextLine(4,"%d",selProg);
	wait1Msec(1000);
	eraseDisplay();
	return selProg;
}

void InitSnatcher(){
	eraseDisplay();
	//drawing logo
	displayCenteredBigTextLine(0,"LEGO");
	displayCenteredBigTextLine(2,"SNATCHER");
	wait1Msec(250);
	// this code already in driver
	//SensorType[gyro] = sensorLightInactive;
	eraseDisplay();
	displayCenteredBigTextLine(0,"LEGO");
	displayCenteredBigTextLine(2,"SNATCHER");
	displayCenteredBigTextLine(4,"InitGyro");
	playSound (soundBeepBeep);
	wait1Msec(500);
	// initing gyro
	while (gyroState == 0){}
	playSound (soundBeepBeep);
}

task main(){
	float fOldAngle = -1;
	InitSnatcher();
	int numberProgram = SelectProgram();
	ClawUp();
	switch (numberProgram){
	case 0: //self-test code
		TNxtButtons new_btnTest= kNoButton;
		bool test = true;
		while(test == true){
			TestSnatcher();
			eraseDisplay();
			displayCenteredBigTextLine(2,"REPEAT");
			displayCenteredBigTextLine(4,"TEST?");
			displayCenteredTextLine(6,"PRESS");
			displayCenteredTextLine(7,"ENTER/EXIT");
			while (nNxtButtonPressed == kNoButton){}
			new_btnTest = nNxtButtonPressed;
			if (new_btnTest == kExitButton) test = false;
		}
		break;
	case 1: //searching objects
		ClawUp();
		int Angle = 0;
		int Distance = 0;
		SearchAngle(&Angle, &Distance, 90, 20);
		eraseDisplay();
		displayTextLine(0, "Angle %d", Angle);
		displayTextLine(2, "Distance %d", Distance);
		wait1Msec(300);
		Rotate(Angle);
		ClawDown();
		MoveStraight(Distance,50);
		ClawUp();
		break;
	case 2: //moving and returning
		ClawUp();
		float dx = 0;
		float dy = 0;
		int dist, delta;
		int ang = 90;
		for(int i = 0; i < move_points; i++){
			delta = (random(8) - 4) * 45;
			ang -= delta;
			Rotate(delta);
			wait1Msec(300);
			dist = (random(8)-4)*10;
			MoveStraight(dist);
			dx += cos(degreesToRadians(ang))*dist;
			dy += sin(degreesToRadians(ang))*dist;
			eraseDisplay();
			displayTextLine(0, "angle= %d", ang);
			displayTextLine(1, "dx=  %.3f", dx);
			displayTextLine(2, "dy=  %.3f", dy);
			wait1Msec(300);
		}
		delta = ang;
		wait1Msec(5000);
		if (dx < 0)
			delta -= radiansToDegrees(atan(dy/dx));
		else if (dx > 0)
			delta -= radiansToDegrees(atan(dy/dx)) + 180;
		else
			delta += (dy/abs(dy))*90;
		delta = (delta % 360);
		if (abs(delta)>180)
			delta -= (delta/abs(delta))*360;
		Rotate(delta);
		ang -= delta;
		wait1Msec(300);
		MoveStraight(ceil(sqrt(dx*dx+dy*dy)));
		wait1Msec(300);
		Rotate((ang-90)%360);
		ang -= (ang-90)%360;
		displayTextLine(3, "angle  %.3f", ang);
		break;
	case 3:// code for wall avoidance
		ClawUp();
		MoveStraight(50,50);
		while (true){
			new_dest = SensorValue[sonar];
			if (abs(new_dest-old_dest)>10) eraseDisplay();
			displayBigStringAt(0, 60, "Distance");
			displayBigStringAt(0, 40, "%d cm", new_dest);
			old_dest = new_dest;
			if (new_dest<30) {
				playSound (soundBeepBeep);
				Rotate(180,50);
				MoveStraight(50,50);
			}
		}
		break;
	case 4:// code for selecting only green and blue balls
		ClawDown();
		while (true){
			new_color = SensorValue[color];
			if ((new_color == GREENCOLOR)||(new_color == BLUECOLOR)){
				playSound (soundBeepBeep);
				ClawUp();
				wait1Msec(3000);
				ClawDown();
				wait1Msec(3000);
			}
		}
		break;
	case 5: //code for view sonar
		ClawUp();
		old_dest = 0;
		while (true){
			new_dest = SensorValue[sonar];
			if (new_dest != old_dest){
				eraseDisplay();
				displayCenteredBigTextLine(0, "View");
				displayCenteredBigTextLine(2, "distance");
				displayCenteredBigTextLine(4, "%d cm", new_dest);
				old_dest = new_dest;
			}
			wait1Msec(5);
		}
		break;
	case 6: //code for view gyro
		fOldAngle = -100;
		while(true){
			if (gyroAngle != fOldAngle){
				eraseDisplay();
				displayCenteredBigTextLine(0, "View");
				displayCenteredBigTextLine(2, "angle");
				displayCenteredBigTextLine(4, "%.3f", gyroAngle);
				fOldAngle = gyroAngle;
			}
			wait1Msec(5);
		}
		break;
	case 7:// code for view color
		old_color = REDCOLOR;
		ClawDown();
		while (true){
			new_color = SensorValue[color];
			if (new_color != old_color){
				eraseDisplay();
				displayCenteredBigTextLine(0, "View");
				displayCenteredBigTextLine(2, "color");
				switch (new_color) {
					case BLACKCOLOR:    displayCenteredBigTextLine(4, "Black"); break;
					case BLUECOLOR:     displayCenteredBigTextLine(4, "Blue"); break;
					case GREENCOLOR:    displayCenteredBigTextLine(4, "Green"); break;
					case YELLOWCOLOR:   displayCenteredBigTextLine(4, "Yellow"); break;
					case REDCOLOR:      displayCenteredBigTextLine(4, "Red"); break;
					case WHITECOLOR:    displayCenteredBigTextLine(4, "White"); break;
					default:            displayCenteredBigTextLine(4, "Undefined"); break;
				}
				old_color = new_color;
			}
			wait1Msec(5);
		}
		break;
	case 8: // code for play all sounds
		int nSoundIndex;
		while (true){
			for (nSoundIndex = 0; nSoundIndex <= 113; ++nSoundIndex){
				displayCenteredBigTextLine(3, "%d", nSoundIndex);
				playSound((TSounds) nSoundIndex);
				if (bSoundActive){
					while (bSoundActive){}
					wait1Msec(500);
				}
			}
		}
		break;
	case 9: // code for view pressed button
		nNxtExitClicks  = 3; // Triple clicking EXIT button will terminate program
		eraseDisplay();
		displayCenteredBigTextLine(0,"View");
		displayCenteredBigTextLine(2,"buttons");
		displayCenteredBigTextLine(4,"Nobutton");
		while (true){
			// wait for button press
			while (nNxtButtonPressed == kNoButton){}
			new_btn = nNxtButtonPressed
			if (new_btn != old_btn){
				eraseDisplay();
				displayCenteredBigTextLine(0,"View");
				displayCenteredBigTextLine(2,"buttons");
				switch (new_btn){
				case kLeftButton:	  displayCenteredBigTextLine(4, "Left"); break;
				case kRightButton:	displayCenteredBigTextLine(4, "Right"); break;
				case kEnterButton:	displayCenteredBigTextLine(4, "Enter") ;break;
				case kExitButton:	  displayCenteredBigTextLine(4, "Exit"); break;
				default:            displayCenteredBigTextLine(4, "Nobutton"); break;
				}
			}
			// wait for button release
			while (nNxtButtonPressed != kNoButton){}
		}
		break;
	default:
		break;
	}
	while(true){}
}
