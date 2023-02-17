task main () {

  eraseDisplay();

  displayTextLine(0, "Types and sizes");
  displayTextLine(1, "byte  %d  ubyte %d", sizeof(byte), sizeof(ubyte));
  displayTextLine(2, "int   %d  short %d", sizeof(int), sizeof(short));
  displayTextLine(3, "bool  %d  word  %d", sizeof(bool), sizeof(word));
  displayTextLine(4, "long  %d  float %d", sizeof(long), sizeof(float));
  displayTextLine(5, "char    %d ", sizeof(char));
  displayTextLine(6, "string  %d ", sizeof(string));

  while(true);
}
