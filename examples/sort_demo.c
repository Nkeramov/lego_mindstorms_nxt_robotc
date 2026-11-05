#define WAIT_TIME    20
#define MAX_NUMBERS  50
#define BAR_WIDTH    2

// timing results: [0]=Bubble [1]=Selection [2]=Insertion [3]=Shell [4]=Quick
float sortingTimes[5] = {0.0, 0.0, 0.0, 0.0, 0.0};

// draw bars in the top part of the screen and the label at the bottom
// called only when animation is on
void drawLines(const int *arr, string functionName) {
    // bars occupy y = 0..MAX_NUMBERS-1 (max 49), so nothing overlaps line 7.
    nxtEraseRect(0, 0, MAX_NUMBERS * BAR_WIDTH, MAX_NUMBERS);
    for (int i = 0; i < MAX_NUMBERS; i++) {
        nxtDrawLine(i * BAR_WIDTH, 0, i * BAR_WIDTH, arr[i]);
    }
    nxtDisplayCenteredTextLine(7, "%s: %.1fs", functionName, time1[T1] / 1000.0);
    wait1Msec(WAIT_TIME);
}

// fill 0..MAX_NUMBERS-1 and shuffle with Fisher-Yates.
void randomiseArray(int *arr, bool animate) {
    string functionName = "Random";
    for (int i = 0; i < MAX_NUMBERS; i++) {
        arr[i] = i;
    }
    for (int i = MAX_NUMBERS - 1; i > 0; i--) {
        int j = random(i + 1);      // 0..i inclusive
        int temp = arr[i];
        arr[i] = arr[j];
        arr[j] = temp;
        if (animate) drawLines(arr, functionName);
    }
}

// bubble sort with early exit. Returns elapsed time in seconds.
float bubbleSort(int *arr, bool animate) {
    string functionName = "Bubble";
    time1[T1] = 0;
    bool swapped;
    for (int i = 0; i < MAX_NUMBERS - 1; i++) {
        swapped = false;
        for (int j = 0; j < MAX_NUMBERS - 1 - i; j++) {
            if (arr[j] > arr[j + 1]) {
                int temp = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = temp;
                swapped = true;
                if (animate) drawLines(arr, functionName);
            }
        }
        if (!swapped) break;
    }
    return time1[T1] / 1000.0;
}

// selection sort: find the minimum and swap it into place.
// few comparisons-per-write ratio a?? draws only on the n-1 swaps.
float selectionSort(int *arr, bool animate) {
    string functionName = "Selection";
    time1[T1] = 0;
    for (int i = 0; i < MAX_NUMBERS - 1; i++) {
        int minIdx = i;
        for (int j = i + 1; j < MAX_NUMBERS; j++) {
            if (arr[j] < arr[minIdx]) {
                minIdx = j;
            }
        }
        if (minIdx != i) {
            int temp = arr[i];
            arr[i] = arr[minIdx];
            arr[minIdx] = temp;
            if (animate) drawLines(arr, functionName);
        }
    }
    return time1[T1] / 1000.0;
}

// classic insertion sort. Returns elapsed time in seconds.
float insertionSort(int *arr, bool animate) {
    string functionName = "Insertion";
    time1[T1] = 0;
    for (int i = 1; i < MAX_NUMBERS; i++) {
        int key = arr[i];
        int j = i - 1;
        while (j >= 0 && arr[j] > key) {
            arr[j + 1] = arr[j];
            j--;
            if (animate) drawLines(arr, functionName);
        }
        arr[j + 1] = key;
        if (animate) drawLines(arr, functionName);
    }
    return time1[T1] / 1000.0;
}

// shell sort. Returns elapsed time in seconds.
float shellSort(int *arr, bool animate) {
    string functionName = "Shell";
    time1[T1] = 0;
    for (int gap = MAX_NUMBERS / 2; gap > 0; gap /= 2) {
        for (int j = gap; j < MAX_NUMBERS; j++) {
            for (int i = j - gap; i >= 0; i -= gap) {
                if (arr[i + gap] >= arr[i]) break;
                int temp = arr[i];
                arr[i] = arr[i + gap];
                arr[i + gap] = temp;
                if (animate) drawLines(arr, functionName);
            }
        }
    }
    return time1[T1] / 1000.0;
}

// recursive quicksort helper (Lomuto partition).
// for MAX_NUMBERS = 50 the recursion depth is at most 50 and
// typically ~6 on random data a?? well within the NXT task stack.
void quickSortHelper(int *arr, int lo, int hi, bool animate) {
    if (lo >= hi) return;
		string functionName = "Quick";
    int pivot = arr[hi];
    int i = lo - 1;
    for (int j = lo; j < hi; j++) {
        if (arr[j] <= pivot) {
            i++;
            int temp = arr[i];
            arr[i] = arr[j];
            arr[j] = temp;
            if (animate) drawLines(arr, functionName);
        }
    }
    i++;
    int temp = arr[i];
    arr[i] = arr[hi];
    arr[hi] = temp;
    if (animate) drawLines(arr, functionName);
    quickSortHelper(arr, lo, i - 1, animate);
    quickSortHelper(arr, i + 1, hi, animate);
}

float quickSort(int *arr, bool animate) {
    time1[T1] = 0;
    quickSortHelper(arr, 0, MAX_NUMBERS - 1, animate);
    return time1[T1] / 1000.0;
}

task main() {
    int arr[MAX_NUMBERS];
    int copy[MAX_NUMBERS];
    eraseDisplay();
    while (true) {
        // 1) Fill with random data
        randomiseArray(arr, false);
        wait1Msec(500);

        // 2) Dry run a?? honest timings, no drawing, no waits
        memcpy(copy, arr, sizeof(copy)); sortingTimes[0] = bubbleSort(copy, false);
        memcpy(copy, arr, sizeof(copy)); sortingTimes[1] = selectionSort(copy, false);
        memcpy(copy, arr, sizeof(copy)); sortingTimes[2] = insertionSort(copy, false);
        memcpy(copy, arr, sizeof(copy)); sortingTimes[3] = shellSort(copy, false);
        memcpy(copy, arr, sizeof(copy)); sortingTimes[4] = quickSort(copy, false);

        // 3) Visual run a?? animation for the demonstration
        playSound(soundFastUpwardTones);
        memcpy(copy, arr, sizeof(copy)); bubbleSort(copy, true);
        wait1Msec(500);
        memcpy(copy, arr, sizeof(copy)); selectionSort(copy, true);
        wait1Msec(500);
        memcpy(copy, arr, sizeof(copy)); insertionSort(copy, true);
        wait1Msec(500);
        memcpy(copy, arr, sizeof(copy)); shellSort(copy, true);
        wait1Msec(500);
        memcpy(copy, arr, sizeof(copy)); quickSort(copy, true);
        wait1Msec(500);

        // 4) Show results
        playSound(soundDownwardTones);
        eraseDisplay();
        nxtDisplayCenteredTextLine(0, "Sorting times:");
        nxtDisplayTextLine(1, "Bubble:    %.1fs", sortingTimes[0]);
        nxtDisplayTextLine(2, "Selection: %.1fs", sortingTimes[1]);
        nxtDisplayTextLine(3, "Insertion: %.1fs", sortingTimes[2]);
        nxtDisplayTextLine(4, "Shell:     %.1fs", sortingTimes[3]);
        nxtDisplayTextLine(5, "Quick:     %.1fs", sortingTimes[4]);
        nxtDisplayTextLine(7, "[ENTER] to reset");

        while (nNxtButtonPressed != kEnterButton) EndTimeSlice();
        while (nNxtButtonPressed != kNoButton)    EndTimeSlice();
    }
}
