const byte b=5;

task main()
{
	ubyte bstate[b]={0,0,0,0,1};
	while (true){
  	cCmdMessageRead(bstate,b,mailbox3);
  	eraseDisplay();
  	displayTextLine(0,"%d",bstate[0]);
		displayTextLine(1,"%d",bstate[1]);
		displayTextLine(2,"%d",bstate[2]);
		displayTextLine(3,"%d",bstate[3]);
		displayTextLine(4,"%d",bstate[4]);
  }
}
