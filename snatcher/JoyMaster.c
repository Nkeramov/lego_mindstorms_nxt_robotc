	#pragma config(Sensor, S1,     Down,		sensorTouch)
	#pragma config(Sensor, S2,     Left,		sensorTouch)
	#pragma config(Sensor, S3,     Right,		sensorTouch)
	#pragma config(Sensor, S4,     Up,			sensorTouch)
// ================================================================
	const int BT_DATA_SIZE = 8;
	const int BT_MASTER_MODE = 1;
	const int BT_SLAVE_MODE = 2;
	// 4 buttons, claw state, speed and 2 reserved bits
	ubyte bstate[BT_DATA_SIZE] = {0,  0,  0,  0,  1,  50, 0, 0};
	ubyte bstate_old[BT_DATA_SIZE] = {0,  0,  0,  0,  1,  50, 0, 0};
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
void DrawInterface(){
  	eraseDisplay();
  	DrawBTlogo(BT_SLAVE_MODE);
		displayStringAt(2, 40, "Speed");
		displayStringAt(8, 30, "%d", bstate[5]);
		displayStringAt(2, 20, "Claw");
		if (bstate[4])
			displayStringAt(8, 10, "UP");
		else
			displayStringAt(8, 10, "DOWN");

		int _size = 12;
		int _centerX = 65;
		int _centerY = 20;
		// left
		if (bstate[0])
			fillRect(_centerX - 2 * _size, _centerY - _size / 2, _centerX - 1 * _size, _centerY + _size / 2);
		else
			drawRect(_centerX - 2 * _size, _centerY - _size / 2, _centerX - 1 * _size, _centerY + _size / 2);
		// right
		if (bstate[1])
			fillRect(_centerX + 1 * _size, _centerY - _size / 2, _centerX + 2 * _size, _centerY + _size / 2);
		else
			drawRect(_centerX + 1 * _size, _centerY - _size / 2, _centerX + 2 * _size, _centerY + _size / 2);
		// up
		if (bstate[2])
			fillRect(_centerX - _size / 2, _centerY + _size * 2 - 6, _centerX + _size / 2, _centerY + _size * 1 - 6);
		else
			drawRect(_centerX - _size / 2, _centerY + _size * 2 - 6, _centerX + _size / 2, _centerY + _size * 1 - 6);
		// down
		if (bstate[3])
			fillRect(_centerX - _size / 2, _centerY - _size * 1 + 6, _centerX + _size / 2, _centerY - _size * 2 + 6);
		else
			drawRect(_centerX - _size / 2, _centerY - _size * 1 + 6, _centerX + _size / 2, _centerY - _size * 2 + 6);
}
// ================================================================

task main()
{
	setSessionPIN("1234");
	eraseDisplay();
			if (nBTCurrentStreamIndex >= 0)
			btDisconnectAll();
	while (nBTCurrentStreamIndex != -1) {wait1Msec(5);}
	displayCenteredBigTextLine(0, "WAIT");
	displayCenteredBigTextLine(2, "FOR");
	displayCenteredBigTextLine(4, "CONNECT");
	while(nBTCurrentStreamIndex < 0){
  	wait1Msec(10);
  }
	DrawInterface();
	playSoundFile("link.rso");
	while (true){
  	bstate[0] = SensorValue[Left];		// port S2 left
  	bstate[1] = SensorValue[Right];		// port S3 right
  	bstate[2] = SensorValue[Up];			// port S4 up
  	bstate[3] = SensorValue[Down];		// port S1 down
  	TNxtButtons curBtn = nNxtButtonPressed;
  	if (curBtn != kNoButton){
			switch (curBtn){
				case kLeftButton:
					bstate[5] -= 5;
					if (bstate[5] < 10)
						bstate[5] = 10;
					playSound(soundBlip);
				break;
				case kRightButton:
					bstate[5] += 5;
					if (bstate[5] > 100)
						bstate[5] = 100;
					playSound(soundBlip);
				break;
				case kEnterButton:
					if (bstate[4])
						bstate[4] = 0;
					else
						bstate[4] = 1;
					playSound(soundBlip);
				break;
			}
	  	waitUntil(nNxtButtonPressed == kNoButton);
		}
		/*
		cCmdMessageRead(bstate_old, BT_DATA_SIZE, mailbox1);
  	wait1Msec(10);
    while(bBTBusy){
   		wait1Msec(1);
   	}
   	*/
   	DrawInterface();
		cCmdMessageRead(bstate_old, BT_DATA_SIZE, mailbox1);
		wait1Msec(5);


		cCmdMessageWriteToBluetooth(0, bstate, BT_DATA_SIZE, mailbox2);
  	wait1Msec(5);

  }
}
