#include <Servo.h>

int pixels[128];
int slopes[128];

// fundtion to read the values given by the line cam into the array.
void readValues(int array[])
{
  loop through pixels
    pulse CLK
    analogread the cam into pixels.
}
// function that saves slope values given the saved pixel values.
void calcSecant(int array[])
{
  loop through slopes
    slopes[i] = (pixels[i+SLOPELENGTH] - pixels[i]);
}
// function that finds the greater boundary of the line.
int indexOfGreatest(int array[])
{
  loop through slopes
    return index of maximum
}
// function that finds the lower boundary of the line. (similar code to above)
int indexOfLeast(int array[]);

// we should probably look into finding a better algorithm
// to determine the actual edge point. 
// this currently has a problem because with a large slope length, 
// there should be multiple values of near the edge with similar slopes
// and we should choose the value of the slope just before it changes from 
// high slope to low slope. 

// changes: 

// finds proportional error. k_p is experimental.
void P_controller() {
  prop = k_p * error;
}

// finds derivative error. k_d is experimental.
// dt is the time between each reading. that must be exp. determined.
void D_controller() {
  deriv = k_d * (error - prev_Error) / dt;
}

// uses PID algorithm and above functions to determine total error of this reading.
int PID(int midpoint) {
  error = setpoint - midpoint; // finds our current error.
  P_controller();
  D_controller();
  prev_Error = error;   //	Save total error for next loop;
  error = prop + deriv; //	Sum all errors;
  return error; // this error is the change we send to our servo to change the angle accordingly.
}

void loop()
{
  readValues(pixels);
  calcSecant(pixels);
  int startLine = indexOfGreatest(slopes);
  int endLine = indexOfLeast(slopes);
  int midLine = (startLine + endLine)/2;
  setServo(PID(midLine));
}
