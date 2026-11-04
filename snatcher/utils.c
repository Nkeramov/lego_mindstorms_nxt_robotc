TNxtButtons waitForButtonPress() {
  while(nNxtButtonPressed == kNoButton) wait1Msec(10);
  return nNxtButtonPressed;
}

void waitForButtonRelease() {
  while(nNxtButtonPressed != kNoButton) wait1Msec(10);
}

int clamp(int val, int min, int max) {
	return (val < min) ? min : ((val > max) ? max : val);
}
