task main()
{
nxtEnableHSPort();
nxtSetHSBaudRate(9600);  // can go as high as 921600 BAUD
nxtHS_Mode = hsRawMode;
//ubyte data[5] = {'h', 'e', 'l', 'l', 'o'};
//nxtWriteRawHS(data[0], 5, 0);
ubyte reply[5];
char replychar[6]
string s;
int i=0;
while (true){
while (nxtGetAvailHSBytes() < 5) EndTimeSlice(); // wait for the two bytes to come in.
nxtReadRawHS(&reply[0], 5);
eraseDisplay();
for (i=0;i<5;i++){replychar[i]=(char)reply[i];}
replychar[5]=(char)0;
StringFromChars(s,replychar);
nxtDisplayBigTextLine(0,"%s",s);
wait1Msec(5000);

}

}
