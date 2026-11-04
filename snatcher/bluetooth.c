const int BT_MODE_MASTER = 1;
const int BT_MODE_SLAVE = 2;

const int BT_DATA_SIZE = 8;
// 4 buttons, claw state, speed, distance, color
// start claw state is up
// start motors speed is 50
ubyte btData[BT_DATA_SIZE] = {0, 0, 0, 0, 1, 50, 0, 0};
ubyte btDataOld[BT_DATA_SIZE] = {0, 0, 0, 0, 1, 50, 0, 0};


void drawBluetoothLogo(ubyte mode){
	drawLine(84 - 80, 63, 84 - 80, 45);
	drawLine(84 - 80, 63, 90 - 80, 57);
	drawLine(90 - 80, 57, 84 - 80, 54);
	drawLine(84 - 80, 54, 90 - 80, 51);
	drawLine(90 - 80, 51, 84 - 80, 45);
	drawLine(85 - 80, 62, 85 - 80, 46);
	drawLine(86 - 80, 61, 86 - 80, 47);
	drawLine(87 - 80, 60, 87 - 80, 48);
	drawLine(88 - 80, 59, 88 - 80, 49);
	drawLine(89 - 80, 58, 89 - 80, 50);
	invertLine(87 - 80, 54, 87 - 80, 54);
	invertLine(88 - 80, 53, 88 - 80, 55);
	invertLine(89 - 80, 53, 89 - 80, 55);
	drawTextAt(15, 60, "Bluetooth");
	if (mode == BT_MODE_MASTER)
		drawTextAt(15, 52, "Master mode");
	else
		if (mode == BT_MODE_SLAVE)
			drawTextAt(15, 52, "Slave mode");
	drawLine(84 - 80, 53, 84 - 80 - 4, 51);
	drawLine(84 - 80, 52, 84 - 80 - 4, 50);
	drawLine(84 - 80, 55, 84 - 80 - 4, 57);
	drawLine(84 - 80, 56, 84 - 80 - 4, 58);
}

void drawInterface(ubyte mode, ubyte claw = 0, ubyte speed = 0, ubyte distance = 0, ubyte colorCode = 0){
  	eraseDisplay();
  	drawBluetoothLogo(mode);
  	displayStringAt(0, 40, "Voltage %.1f", nAvgBatteryLevel / 1000.0);
		displayStringAt(0, 32, "Speed %d", speed);
		displayStringAt(0, 24, "Claw %s", (claw) ? "UP" : "DOWN");
		displayStringAt(0, 16, "Distance %d", distance);
		switch (colorCode){
			case  BLACKCOLOR: displayStringAt(0, 8, "Color Black"); break;
			case  BLUECOLOR:	displayStringAt(0, 8, "Color Blue"); break;
			case  GREENCOLOR:	displayStringAt(0, 8, "Color Green"); break;
			case  YELLOWCOLOR: displayStringAt(0, 8, "Color Yellow"); break;
			case  REDCOLOR: displayStringAt(0, 8, "Color Red"); break;
			case  WHITECOLOR: displayStringAt(0, 8, "Color White"); break;
			default: displayStringAt(0, 8, "Color Undefined"); break;
		}

		int _size = 10, _centerX = 78, _centerY = 25;
		// left
		if (btData[0])
			fillRect(_centerX - 2 * _size + 4, _centerY - _size / 2, _centerX - 1 * _size + 4, _centerY + _size / 2);
		else
			drawRect(_centerX - 2 * _size + 4, _centerY - _size / 2, _centerX - 1 * _size + 4, _centerY + _size / 2);
		// right
		if (btData[1])
			fillRect(_centerX + 1 * _size, _centerY - _size / 2, _centerX + 2 * _size, _centerY + _size / 2);
		else
			drawRect(_centerX + 1 * _size, _centerY - _size / 2, _centerX + 2 * _size, _centerY + _size / 2);
		// up
		if (btData[2])
			fillRect(_centerX - _size / 2 + 2, _centerY + _size * 2 - _size / 2, _centerX + _size / 2 + 2, _centerY + _size * 1 - _size / 2);
		else
			drawRect(_centerX - _size / 2 + 2, _centerY + _size * 2 - _size / 2, _centerX + _size / 2 + 2, _centerY + _size * 1 - _size / 2);
		// down
		if (btData[3])
			fillRect(_centerX - _size / 2 + 2, _centerY - _size * 1 + _size / 2, _centerX + _size / 2 + 2, _centerY - _size * 2 + _size / 2);
		else
			drawRect(_centerX - _size / 2 + 2, _centerY - _size * 1 + _size / 2, _centerX + _size / 2 + 2, _centerY - _size * 2 + _size / 2);
}

void initBluetooth(){
	btDisconnectAll();
	setSessionPIN("1234");
 	wait1Msec(500);
  if (nBTCurrentStreamIndex >= 0)
    btDisconnectAll();
	while (nBTCurrentStreamIndex != -1) {
		wait1Msec(50);
	}
}

void waitBluetoothConnected(){
	while(nBTCurrentStreamIndex < 0){
  	wait1Msec(50);
  }
}
