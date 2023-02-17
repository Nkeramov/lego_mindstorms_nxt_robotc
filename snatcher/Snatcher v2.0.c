#pragma config(Sensor, S1,     touch,          sensorTouch)
#pragma config(Sensor, S2,     gyro,           sensorI2CHiTechnicGyro)
#pragma config(Sensor, S3,     color,          sensorColorNxtFULL)
#pragma config(Sensor, S4,     sonar,          sensorSONAR)

const int gyro_average_count = 3;
const int sonar_average_count = 10;
const int move_points = 5;
const tSensors kGyroSensor = S2;
const float kMove = 0.029;
//0.037 - wheels
//0.029 - caterpillars

#include "HiTechnic_old_gyroDriver.c"

TButtons new_btn = -1;
TButtons old_btn = -1;
TColors new_color = BLACKCOLOR;
TColors old_color = BLACKCOLOR;
int new_dest = 0;
int old_dest = 0;

void ClearMotor(tMotor mname){
	motor[mname] = 0;
	nMotorEncoder[mname] = 0;
	nMotorEncoderTarget[mname] = 0;
}

void MoveStraight(int dist,int speed=40){
	if (dist*speed<0) speed = -abs(speed);
	dist = abs(dist);
	ClearMotor(motorB);
	ClearMotor(motorC);
	nSyncedMotors = synchBC ;
	nSyncedTurnRatio = 100;
	motor[motorB] = speed;
	while(abs(nMotorEncoder[motorB])*kMove<dist);
	motor[motorB] = 0;
	nSyncedMotors=synchNone;
	ClearMotor(motorB);
	ClearMotor(motorC);
}

void ClawUp(int speed=40){
	ClearMotor(motorA);
	motor[motorA] = speed;
	while(SensorValue[touch] == 0);
	ClearMotor(motorA);
	wait1Msec(500);
}

void ClawDown(int speed=30){
	ClearMotor(motorA);
	speed *=-1;
	motor[motorA] = speed;
	while (abs(nMotorEncoder[motorA]) < 400);
	ClearMotor(motorA);
}

void Rotate(float angle = 180, int speed=30){
	ClearMotor(motorB);
	ClearMotor(motorC);
	speed=abs(speed);
	if(angle>0) speed*=-1;
	angle = abs(angle);
	angle -= 2.1 - (speed/100);
	fGyroAngle = 0;
	nSyncedMotors = synchBC;
	nSyncedTurnRatio = -100;
	motor[motorB] = speed;
	while (abs(fGyroAngle) < 1.5 * angle);
	motor[motorB]=0;
	nSyncedMotors=synchNone;
	ClearMotor(motorB);
	ClearMotor(motorC);
}

int GetDistance(){
	int count = 0;
	int sum = 0;
	int dist = 0;
	while (count < sonar_average_count){
		dist = SensorValue[sonar];
		if (!((dist == 255) && ((sum / (count+1)) < 100) && (sum != 0))){
			count++;
			sum += dist;
		}
		wait1Msec(200);
	}
	return ceil(sum / sonar_average_count);
}

void SearchAngle(int *ang, int *dist,int angle_max = 90, int speed = 30){
	angle_max = abs(angle_max);
	speed = abs(speed);
	int angle_min = 0;
	int dist_new = 0;
	int dist_min = 255;
	int dist_old = 255;
	int angle_old = 0;
	bool f1 = false;
	bool f2 = false;
	Rotate(-angle_max,speed);
	wait1Msec(300);
	fGyroAngle = -angle_max*1.5;
	angle_max -= 2.1 - (speed/100);
	nSyncedMotors = synchBC;
	nSyncedTurnRatio = -100;
	motor[motorB] = -speed;
	while (fGyroAngle<1.5*angle_max || f1){
		dist_new = SensorValue[sonar];
		if (!f1 && (dist_new - dist_min < 5) && (dist_new < 100)){
			f1 = true;
			angle_old = ceil(fGyroAngle * 0.66);
			dist_old = dist_new;
		}
		else
			if (f1 && (abs(dist_new - dist_old) > 5)){
			f1 = false;
			if (f2) angle_min = ceil((fGyroAngle + 1.5 * angle_old) * 0.33);
		}
		if (f1 && (dist_new < dist_min)){
			dist_min = dist_new;
			f2 = true;
		}
		else
			if (!f1) f2 = false;
		/*if(f1 && abs(dist_new-dist_min)>5){
		f1 = false;
		angle_min = ceil((fGyroAngle+1.5*angle_old)*0.33);
		}
		else if(abs(dist_new - dist_min)<=5){
		if(f1)
		angle_old = ceil(fGyroAngle*0.66);
		dist_old = dist_new;
		else{
		f1 = true;
		angle_old = angle_min;
		}
		}*/
	}
	motor[motorB]=0;
	nSyncedMotors=synchNone;
	ClearMotor(motorB);
	ClearMotor(motorC);
	wait1Msec(300);
	Rotate(ceil(-fGyroAngle*0.66), speed);
	*ang = angle_min-10;
	*dist = dist_min-10;
}

void testMotors(){
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

int selectProgram(int count){
	eraseDisplay();
	//drawing logo
	displayCenteredBigTextLine(0,"LEGO");
	displayCenteredBigTextLine(2,"SNATCHER");
	wait1Msec(250);
	bool exit = false;
	// double clicking EXIT button will terminate program
	nNxtExitClicks  = 2;
	// this code already in driver
	//SensorType[gyro] = sensorLightInactive;
	eraseDisplay();
	displayCenteredBigTextLine(0,"LEGO");
	displayCenteredBigTextLine(2,"SNATCHER");
	displayCenteredBigTextLine(4,"InitGyro");
	playSound (soundBeepBeep);
	wait1Msec(500);
	// initing gyro
	while (!bIsGyroInitialized())
	{
		if (bIsGyroInitialized()) break;
	}
	int ctype = -1;
	int i = 0;
	playSound (soundBeepBeep);
	eraseDisplay();
	displayCenteredBigTextLine(0,"SELECT");
	displayCenteredBigTextLine(2,"PROGRAM");
	displayCenteredBigTextLine(4,"%d",i);
	displayCenteredTextLine(6,"test motors");
	displayCenteredTextLine(7,"and claw");
	// selecting program
	while (ctype < 0){
		// wait for button press
		while (true){
			new_btn = nNxtButtonPressed;
			if (new_btn != -1)
				break;
		}
		playSound(soundBlip);
		switch (new_btn){
		case kLeftButton: i--; if (i < 0) i = 9;	break;
		case kRightButton:	i++; if (i > 9) i = 0;	break;
		case kEnterButton:	ctype = i; break;
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
		displayCenteredBigTextLine(4,"%d",i);
		switch (i){
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
		while (true){
			new_btn = nNxtButtonPressed;
			if (new_btn == -1) break;
		}
	}
	// program selected go to perform it
	eraseDisplay();
	displayCenteredBigTextLine(2,"SELECTED");
	displayCenteredBigTextLine(4,"%d",ctype);
	wait1Msec(1000);
	eraseDisplay();
	return ctype;
}

task main(){
	float fOldAngle = -1;
	int numberProgram = selectProgram(9);
	ClawUp();
	switch (numberProgram){
	case 0: //self-test code
		TButtons new_btnTest=-1;
		bool test = true;
		while(test == true){
			testMotors();
			eraseDisplay();
			displayCenteredBigTextLine(2,"REPEAT");
			displayCenteredBigTextLine(4,"TEST?");
			displayCenteredTextLine(6,"PRESS");
			displayCenteredTextLine(7,"ENTER/EXIT");
			while (true){
				new_btnTest = nNxtButtonPressed;
				if (new_btnTest != -1) break;
			}
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
		while(true);
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
		while(true);

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
			if (new_dest != old_dest)
			eraseDisplay();
			displayCenteredBigTextLine(0, "View");
			displayCenteredBigTextLine(2, "distance");
			displayCenteredBigTextLine(4, "%d cm", new_dest);
			old_dest = new_dest;
		}
		break;
	case 6: //code for view gyro
		fOldAngle = -100;
		while(true){
			if (fGyroAngle != fOldAngle){
				eraseDisplay();
				displayCenteredBigTextLine(0, "View");
				displayCenteredBigTextLine(2, "angle");
				displayCenteredBigTextLine(4, "%d", (long) (fGyroAngle*10/15));
				fOldAngle = fGyroAngle;
			}
			wait1Msec(3);
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
		}
		break;
	case 8: // code for play all sounds
		int nSoundIndex;
		while (true){
			for (nSoundIndex = 0; nSoundIndex <= 113; ++nSoundIndex){
				nxtDisplayCenteredBigTextLine(3, "%d", nSoundIndex);
				playSound((TSounds) nSoundIndex);
				if (bSoundActive){
					while (bSoundActive);
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
			while (true){
				new_btn = nNxtButtonPressed;
				if (new_btn != -1) break;
			}
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
			while (true){
				new_btn = nNxtButtonPressed;
				if (new_btn == -1) break;
			}
		}
		break;
	default:
		break;
	}
}
