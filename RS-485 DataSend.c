task main()
{
nxtEnableHSPort();
nxtSetHSBaudRate(9600);  // can go as high as 921600 BAUD
nxtHS_Mode = hsRawMode;
ubyte data[5] = {'h', 'e', 'l', 'l', 'o'};

//ubyte reply[2]
//while (nxtGetAvailHSBytes() < 2) EndTimeSlice(); // wait for the two bytes to come in.
//nxtReadRawHS(reply[0], 2);
while(true){

nxtWriteRawHS(&data[0], 5, 0);
wait1Msec(5000);

}
}
