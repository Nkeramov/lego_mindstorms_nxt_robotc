TNxtButtons waitForButtonPress() {
  while(nNxtButtonPressed == kNoButton) wait1Msec(10);
  return nNxtButtonPressed;
}

void waitForButtonRelease() {
  while(nNxtButtonPressed != kNoButton) wait1Msec(10);
}

ubyte uclamp(ubyte val, ubyte min, ubyte max) {
	return (val < min) ? min : ((val > max) ? max : val);
}

byte clamp(byte val, byte min, byte max) {
	return (val < min) ? min : ((val > max) ? max : val);
}
