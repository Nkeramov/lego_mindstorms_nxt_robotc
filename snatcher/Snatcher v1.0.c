#pragma config(Sensor, S1,     touchSensor1,   sensorTouch)
#pragma config(Sensor, S2,     touchSensor2,   sensorTouch)
#pragma config(Sensor, S3,     colorSensor,    sensorCOLORFULL)
#pragma config(Sensor, S4,     sonarSensor,    sensorSONAR)

/*
To check which button is pressed, use variable nNxtButtonPressed. Its values are as follows: -1
= no button pressed; 0 = Gray Rectangle button; 1 = Right Arrow button; 2 = Left Arrow button;
3 = Orange Square button.
nSyncedMotors=synchNone;.
nNxtButtonPressed
nxtDisplayTextLine

/ Characters in standard firmware use a 6W x 8H FONT.
//
// ROBOTC also supports a larger font of 12W x 16H pixels.
*/

void ClearMotorA(){
	motor[motorA] = 0;
	nMotorEncoder[motorA] = 0;
  nMotorEncoderTarget[motorA] =0;
}

void ClearMotorB(){
	motor[motorB] = 0;
	nMotorEncoder[motorB] = 0;
  nMotorEncoderTarget[motorB] =0;
}

void ClearMotorC(){
	motor[motorC] = 0;
	nMotorEncoder[motorC] = 0;
  nMotorEncoderTarget[motorC] =0;
}

void MoveLeft(int targ){
	int d=(537/90) * targ;
	ClearMotorB();
	ClearMotorC();
  motor[motorB] = 30;
  motor[motorC] = -30;
  while (nMotorEncoder[motorB]<=d){
  	nMotorEncoderTarget[motorB] = nMotorEncoder[motorB]+2;
  	nMotorEncoderTarget[motorC] = nMotorEncoder[motorC]+2;
  }
  motor[motorB] = 0;
  motor[motorC] = 0;
}

void MoveRight(int targ){
	int d=(457/90) * targ;
	ClearMotorB();
	ClearMotorC();
  motor[motorB] = -30;
  motor[motorC] = 30;
  while (nMotorEncoder[motorC]<=d){
  	nMotorEncoderTarget[motorB] = nMotorEncoder[motorB]+2;
  	nMotorEncoderTarget[motorC] = nMotorEncoder[motorC]+2;
  }
  motor[motorB] = 0;
  motor[motorC] = 0;
}

void MoveUp(){
	ClearMotorA();
  motor[motorA] = 50;
	while(SensorValue(touchSensor1) == 0){
		nMotorEncoderTarget[motorA] = nMotorEncoder[motorA]+2;
	}
	nMotorEncoderTarget[motorA] = nMotorEncoder[motorA]+5;
}

void MoveDown(){
	ClearMotorA();
	motor[motorA] = -50;
	//while(SensorValue(touchSensor2) == 0){
	//while(SensorValue(sonarSensor) == 255){
	  while(SensorValue(sonarSensor) > 200){
		nMotorEncoderTarget[motorA] = nMotorEncoder[motorA]-2;
	}
}

void MoveForward(int power,int delay){
	ClearMotorB();
	ClearMotorC();
	motor[motorB] = power;
	motor[motorC] = power;
	if (delay>0) wait1Msec(delay);
}

void MoveBack(int power,int delay){
	ClearMotorB();
	ClearMotorC();
	motor[motorB] = -power;
	motor[motorC] = -power;
	if (delay>0) wait1Msec(delay);
}

task main()
{
  int new_dest=0;
  int old_dest=0;
  MoveDown();
	wait1Msec(1000);
	MoveUp();
	wait1Msec(1000);
	MoveDown();
	wait1Msec(1000);
  //MoveForward(50,0);
	TColors new_color=BLACKCOLOR;
	TColors old_color=BLACKCOLOR;
	TButtons new_btn=-1;
	TButtons old_btn=-1;

	int min_dest = 255;
	int min_angle = 0;
	int angle = 0;
	old_dest = 255;
	MoveUp();
	bool f=true;
	MoveRight(90);
	wait1Msec(2000);
	MoveRight(90);
	/*
	while(f==true){
	  new_dest = SensorValue[sonarSensor];
	  if (new_dest<min_dest) {
	    min_dest = new_dest;
      min_angle =  angle;
	  }
	  angle += 10;
	  MoveRight(10);
	  if (angle == 360) {
	     f = false;
	     PlaySound (soundBeepBeep);
	  }
	  wait1Msec(2000);
	  if (abs(new_dest-old_dest)>10) eraseDisplay();
		nxtDisplayBigStringAt(0, 60, "Distance");
		nxtDisplayBigStringAt(0, 40, "%d cm", min_dest);
		old_dest = new_dest;
  }

  while(true){

  }


  */
	/*
	//======================================================
	// play all sounds
	int nSoundIndex;
	while (true){
		for (nSoundIndex = 0; nSoundIndex <= 113; ++nSoundIndex)
		{
			nxtDisplayCenteredBigTextLine(3, "%d", nSoundIndex);
			PlaySound((TSounds) nSoundIndex);
			if (bSoundActive)
			{
				while (bSoundActive)
				{
					wait1Msec(1);
				}
				wait1Msec(500);
			}
		}
	}
  */
	//======================================================
	// code for button press detection
	/*
	nNxtExitClicks  = 3; // Triple clicking EXIT button will terminate program
	eraseDisplay();
	nxtDisplayBigStringAt(0, 60, "Button");
	nxtDisplayBigStringAt(0, 40, "Nobutton");
	while (true){
	  // wait for button press
	  while (true){
		  	new_btn = nNxtButtonPressed;
		  	if (new_btn != -1)
		  	  break;
		}
    if (new_btn != old_btn){
	      eraseDisplay();
				nxtDisplayBigStringAt(0, 60, "Button");
			  switch (new_btn)
			  {
			  	case kLeftButton:	  nxtDisplayBigStringAt(0, 40, "Left");break;
					case kRightButton:	nxtDisplayBigStringAt(0, 40, "Right");break;
					case kEnterButton:	nxtDisplayBigStringAt(0, 40, "Enter");break;
					case kExitButton:	  nxtDisplayBigStringAt(0, 40, "Exit");break;
					default:            nxtDisplayBigStringAt(0, 40, "Nobutton");break;
				}
	  }
	  while (true) // wait for button release
	  {
	  	new_btn = nNxtButtonPressed;
	  	if (new_btn == -1)
	  	  break;
	  }
  }
  */



	//======================================================
	// code for selecting green and blue balls
  /*
	while (true){
		new_color = SensorValue[colorSensor];
		if ((new_color == GREENCOLOR)||(new_color == BLUECOLOR)){
			  PlaySound (soundBeepBeep);
			  MoveUp();
			  wait1Msec(3000);
			  MoveDown();
			  wait1Msec(3000);
	  }
  }

  //======================================================
  // code for color detection
  /*
	while (true){
    new_color = SensorValue[colorSensor];
    if (new_color != old_color){
      eraseDisplay();
			nxtDisplayBigStringAt(0, 60, "Color");
      switch (new_color)
			{
				case BLACKCOLOR:    nxtDisplayBigStringAt(0, 40, "Black");break;
			  case BLUECOLOR:     nxtDisplayBigStringAt(0, 40, "Blue");break;
			  case GREENCOLOR:    nxtDisplayBigStringAt(0, 40, "Green");break;
			  case YELLOWCOLOR:   nxtDisplayBigStringAt(0, 40, "Yellow");break;
			  case REDCOLOR:      nxtDisplayBigStringAt(0, 40, "Red");break;
			  case WHITECOLOR:    nxtDisplayBigStringAt(0, 40, "White");break;
			  default:            nxtDisplayBigStringAt(0, 40, "Undefined");break;
			}
			old_color = new_color;
	  }
	}
	*/
	//======================================================
	// code for wall avoidance
  /*
  MoveUp();
	MoveForward(50,0);
	while (true){
	  new_dest = SensorValue[sonarSensor];
		if (abs(new_dest-old_dest)>10) eraseDisplay();
		nxtDisplayBigStringAt(0, 60, "Distance");
		nxtDisplayBigStringAt(0, 40, "%d cm", new_dest);
		old_dest = new_dest;
		if (new_dest<30) {
		  PlaySound (soundBeepBeep);
		  MoveRight(180);
		  MoveForward(50,0);
	  }
	}
	//======================================================
	*/
}
