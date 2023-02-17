/**
 * This is a small test program to demonstrate various sorting algorithms.
 *
 * Changelog:
 * - 0.1: Initial release
 *

 * THIS CODE WILL ONLY WORK WITH ROBOTC VERSION 3.00 AND HIGHER.
 * Xander Soldaat (mightor_at_gmail.com)
 * 21 July 2012
 * version 0.1
 */

#define WAIT_TIME 20
#define MAX_NUMBERS 50

// Keep track of our sorting times.
float sortingTimes[3] = {0.0, 0.0, 0.0};

// Draw the lines on the screen after each iteration.  Could be more efficient
// but that would mean I would have to do more thinking...
void drawLines(int *arr, string functionName)
{
  nxtDisplayCenteredTextLine(0, "%s: %.1fs", functionName, time1[T1] / 1000.0);
  nxtEraseRect(0, 0, 100, 50);
  for (int i = 0; i < MAX_NUMBERS; i++)
  {
    nxtDrawLine(i*2, 0, i*2, arr[i]);
  }
  wait1Msec(WAIT_TIME);
}

// Randomise our array by first filling it up with consecutive numbers
// and then mixing them up.
void randomiseArray(int *arr)
{
  int randomPos;

  string functionName = "Random";

  time1[T1] = 0;

  for (int i = 0; i < MAX_NUMBERS; i++)
  {
    arr[i] = i;
  }

  // now randomly mix it up, 2 times.
  for (int i = 0; i < 2; i++)
  {
	  for (int j = 0; j < MAX_NUMBERS; j++)
	  {
	    randomPos = random[MAX_NUMBERS - 1];
	    int temp = arr[j];
	    arr[j] = arr[randomPos];
	    arr[randomPos] = temp;
			drawLines(arr, functionName);
	  }
  }
}

// Do a bubble sort on the numbers, returns the number of seconds taken
float bubbleSort(int *arr)
{
  int i, j;

  string functionName = "Bubble";

  time1[T1] = 0;

  for(i = 0; i < MAX_NUMBERS; i++)
	{
	  for(j = 0; j < (MAX_NUMBERS - 1); j++)
	  {
	    if (arr[j] > arr[j+1])
	    {
				ubyte temp = arr[j];
				arr[j] = arr[j+1];
				arr[j+1] = temp;
				drawLines(arr, functionName);
			}
		}
	}
	return time1[T1] / 1000.0;
}

// Do an insertion sort on the numbers, returns the number of seconds taken
float insertionSort(int *arr)
{
  int i, j, k;

  time1[T1] = 0;

  string functionName = "Insertion";

  for (i = 1 ; i < MAX_NUMBERS ; i++ )
	{
		for (j = 0 ; j < i ; j++ )
		{
			if ( arr[j] > arr[i] )
			{
				int temp = arr[j] ;
				arr[j] = arr[i] ;

				for (k = i ; k > j ; k-- )
				{
					arr[k] = arr[k - 1] ;
					drawLines(arr, functionName);
				}
				arr[k + 1] = temp ;
				drawLines(arr, functionName);
			}
		}
	}
	return time1[T1] / 1000.0;
}


// Do a shell sort on the numbers, returns the number of seconds taken
float shellSort(int *arr)
{
	int j, i, m;
  time1[T1] = 0;
	string functionName = "Shell";

	for(m = MAX_NUMBERS/2 ; m > 0; m /= 2)
	{
		for(j = m; j < MAX_NUMBERS; j++)
		{
			for(i = j - m; i >= 0; i -= m)
			{
				if(arr[i + m] >= arr[i])
					break;
				else
				{
					int mid = arr[i];
					arr[i] = arr[i + m];
					arr[i + m] = mid;
					drawLines(arr, functionName);
				}
			}
		}
	}
	return time1[T1] / 1000.0;
}

// The main task
task main ()
{
  int arr[MAX_NUMBERS];
  int copy[MAX_NUMBERS];

  eraseDisplay();
  while (true)
  {
    // Populate the array with random data
    randomiseArray(arr);

    wait1Msec(1000);
    // Iterate through each sorting algorithm
    for (int i = 0; i < 3; i++)
    {
      // Make a copy of the original and sort that.
	    memcpy(copy, arr, sizeof(arr));
	    playSound(soundFastUpwardTones);

		  switch(i)
		  {
		    case 0: sortingTimes[0] = bubbleSort(copy); break;
		    case 1: sortingTimes[1] = shellSort(copy); break;
		    case 2: sortingTimes[2] = insertionSort(copy); break;
		  }

	    playSound(soundBlip);
		  wait1Msec(1000);
		}

		// Show the results.
		playSound(soundDownwardTones);
		eraseDisplay();
		nxtDisplayCenteredTextLine(0, "Sorting times:");
		nxtDisplayTextLine(2, "Bubble:    %.1fs", sortingTimes[0]);
		nxtDisplayTextLine(3, "Shell:      %.1fs", sortingTimes[1]);
		nxtDisplayTextLine(4, "Insertion: %.1fs", sortingTimes[2]);
		nxtDisplayTextLine(6, "[ENTER] to reset");

		// Press enter to go again.
		while (nNxtButtonPressed != kEnterButton) EndTimeSlice();
		while (nNxtButtonPressed != kNoButton) EndTimeSlice();
	}


}
