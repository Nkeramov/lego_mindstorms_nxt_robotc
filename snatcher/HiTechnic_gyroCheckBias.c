#pragma config(Sensor, S2,     gyro,           sensorI2CHiTechnicGyro)

float gyroBias = 0.0;
const tSensors gyroPort = S2;
const int GYRO_BIAS_SAMPLES = 400;
long samplesSum = 0;


task main(){


	float fOldAngle = -1;
	eraseDisplay();
	for(int i=0;i<8;i++){
		samplesSum=0;
		for (int k = 0; k < GYRO_BIAS_SAMPLES ; k++)
		{
			samplesSum += SensorRaw[gyroPort];
			wait1Msec(10);
		}
		gyroBias = (float) samplesSum / GYRO_BIAS_SAMPLES;
		displayTextLine(i,"%f",gyroBias);
		wait10Msec(6000);
	}
while(true){}

}
