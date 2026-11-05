task main() {
    eraseDisplay();

    // header
    displayTextLine(0, "Type sizes(bytes)");

    // 8-bit integer types
    displayTextLine(1, "byte %d  ubyte  %d", sizeof(byte),  sizeof(ubyte));

    // 16-bit integer types
    displayTextLine(2, "int  %d  short  %d", sizeof(int),   sizeof(short));

    // bool and unsigned 16-bit word
    displayTextLine(3, "bool %d  word   %d", sizeof(bool),  sizeof(word));

    // 32-bit numeric types
    displayTextLine(4, "long %d  float  %d", sizeof(long),  sizeof(float));

    // char is guaranteed to be 1 by the C standard
		displayTextLine(5, "char %d", sizeof(char));

    // string is a typedef for char*, so sizeof(string) is the pointer size (4).
		displayTextLine(6, "string %d", sizeof(string));

    // prompt
    displayCenteredTextLine(7, "[ENTER] to exit");

    // yield to the scheduler; exit cleanly on ENTER.
    while (nNxtButtonPressed != kEnterButton) EndTimeSlice();

    eraseDisplay();
}
