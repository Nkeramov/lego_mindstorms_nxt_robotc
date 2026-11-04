	#pragma config(Sensor, S1,     Down,		sensorTouch)
	#pragma config(Sensor, S2,     Left,		sensorTouch)
	#pragma config(Sensor, S3,     Right,		sensorTouch)
	#pragma config(Sensor, S4,     Up,			sensorTouch)

	#include "bluetooth.c"
	#include "utils.c"

	#define MIN_SPEED 							10
	#define MAX_SPEED 							100

void drawProgressBar(int x, int y, int width, int percent) {
    int fillWidth = (width * percent) / 100;
    drawRect(x, y, x + width, y + 8);
    fillRect(x, y, x + fillWidth, y + 8);
}

task main(){
	eraseDisplay();
	displayCenteredBigTextLine(0, "WAITING");
	displayCenteredBigTextLine(2, "FOR");
	displayCenteredBigTextLine(4, "CONNECT");
	displayCenteredTextLine(7, "SLAVE MODE");
	initBluetooth();
	waitBluetoothConnected();
  playSoundFile("link.rso");
  drawInterface(BT_MODE_SLAVE);
  ubyte speed = 0, distance = 0, color = 0;
  bool claw = false;
  while (true){
		TFileIOResult nBTCmdRdErrorStatus = cCmdMessageRead(btDataOld, BT_DATA_SIZE, mailbox1);
		if (nBTCmdRdErrorStatus == ioRsltSuccess){
			claw = btDataOld[4];
			speed = btDataOld[5];
			distance = btDataOld[6];
			color = btDataOld[7];
		}
		drawInterface(BT_MODE_SLAVE, claw, speed, distance, color);
  	memcpy(btDataOld, btData, BT_DATA_SIZE);
		btData[0] = SensorValue[Left];
  	btData[1] = SensorValue[Right];
  	btData[2] = SensorValue[Up];
  	btData[3] = SensorValue[Down];
  	TNxtButtons curBtn = nNxtButtonPressed;
  	if (curBtn != kNoButton){
			switch (curBtn){
				case kLeftButton:
					btData[5] -= 5;
					if (btData[5] < 10)
						btData[5] = 10;
				  break;
				case kRightButton:
					btData[5] += 5;
					if (btData[5] > 100)
						btData[5] = 100;
				  break;
				case kEnterButton:
				  btData[4] = (btData[4] + 1) % 2;
    			break;
			}
			playSound(soundBlip);
			waitUntil(nNxtButtonPressed == kNoButton);
		}
		if (memcmp(btData, btDataOld, BT_DATA_SIZE) != 0) {
			drawInterface(BT_MODE_SLAVE, claw, speed, distance, color);
			cCmdMessageWriteToBluetooth(0, btData, BT_DATA_SIZE, mailbox2);
		}
		wait1Msec(50);
  }
}
