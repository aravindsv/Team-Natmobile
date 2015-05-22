//#include "Natcar_PID.ino"
#include <Servo.h>

//Motor Control variables
#define motor1 3     //3 for PCB
#define motor2 4     //4 for PCB

// Camera Control Variables. 
#define CLK 20       //16 for Perfboard, 16 for PCB
#define AO 22        //18 for Perfboard, 14 for PCB
#define SI 21        //17 for Perfboard, 15 for PCB
#define SERVO_PIN 10 //10 for Perfboard, 13 for PCB

#define SERVO_CENTER 66 // actually 90, but accounts for cam bias
#define SERVO_RANGE 12
#define SCALINGFACTOR 20
#define THRESHOLD 4
#define SLOPELENGTH 3 // this can be fine tuned in t he future. 
#define SKIPVAL 1

#define EDGE_IGNORE 25
#define ERROR_SIZE 15
#define MOTOR_POWER 35

int mini = 1000;
int maxi = 0;
int pixels[128 / SKIPVAL];
int slopes[128 / SKIPVAL];

const int NUM_SAMPLES = 10;
int PREV_SAMPLES[NUM_SAMPLES];
int curs = 0;

Servo ourServo;

void setup()
{
  pinMode(motor1, OUTPUT);
  pinMode(motor2, OUTPUT);
  pinMode(SI, OUTPUT);
  pinMode(CLK, OUTPUT);
  pinMode(AO, INPUT);
  
  ourServo.attach(SERVO_PIN);
  ourServo.write(SERVO_CENTER);
  digitalWrite(SI, LOW);
  digitalWrite(CLK, LOW);
  digitalWrite(motor2, HIGH);

  Serial.begin(9600);
  
  delay(1000);
}

void calcSecant(int pixels[])
{
  for (int i = 0; i < 128-SLOPELENGTH; i++)
  {
    slopes[i] = (pixels[i+SLOPELENGTH] - pixels[i]);
  }
}

// we should probably look into finding a better algorithm
// to determine the actual edge point. 
// this currently has a problem because with a large slope length, 
// there should be multiple values of near the edge with similar slopes
// and we should choose the value of the slope just before it changes from 
// high slope to low slope. 
int indexOfGreatest(int array[])
{
  int greatest = EDGE_IGNORE;
  for (int i = EDGE_IGNORE; i < 128-SLOPELENGTH - EDGE_IGNORE; i++)
  {
    if (slopes[i] > slopes[greatest])
    {
      greatest = i;
    }
  }
  return greatest;
}

int indexOfLeast(int array[])
{
  int least = 128-SLOPELENGTH-EDGE_IGNORE-1;
  for (int i = 128-SLOPELENGTH-EDGE_IGNORE-1; i >= EDGE_IGNORE; i--)
  {
    if (slopes[i] < slopes[least])
    {
      least = i;
    }
  }
  return least;
}

void displayView(int startIndex, int endIndex, int midIndex)
{
  char disp[128];
  Serial.print("[");
  for (int i = 0; i < 128; i++)
  {
    if (i == startIndex || i == endIndex)
    {
      disp[i] = '|';
    }
    else if (i == midIndex)
    {
      disp[i] = '*';
    }
    else if (i > startIndex && i < endIndex)
    {
      disp[i] = '-';
    }
    else
    {
      disp[i] = ' ';
    }
    Serial.printf("%c", disp[i]);
  }
  Serial.print("]");
}

void setServo(int error)
{
  ourServo.write(SERVO_CENTER-error);
}

void loop()
{
  //Serial.println(millis());
  digitalWrite(SI, HIGH);
  digitalWrite(CLK, HIGH);
  digitalWrite(SI, LOW);
  digitalWrite(CLK, LOW);

  for (int i = 128 / SKIPVAL - 1; i >= 1 ; i-- )
  {
    digitalWrite(CLK, HIGH);
    pixels[i] = analogRead(AO);
    digitalWrite(CLK, LOW);
  }
//  for (int i = 0; i < 128 / SKIPVAL; i++)
//  {
//    if (pixels[i] > maxi)
//      maxi = pixels[i];
//    if (pixels[i] < mini)
//      mini = pixels[i];
//  }
  for (int i = 1; i < 128; i++)
  {
    Serial.print(map(pixels[i], 0, 200, 0, 9));
    Serial.print("");
    //Serial.printf("%d ", pixels[i] / 20 );
  }
//  // Serial.printf("%d\t%d\n", mini, maxi );
//  Serial.println("");
  
//  for (i = 0; i < 128; i++)
//  {
//    Serial.println(pixels[i]);
//  }
//  while(1)
//  {
//    delay(1000);
//  }
  calcSecant(pixels);

  Serial.println();
  int startLine = indexOfGreatest(slopes);
  int endLine = indexOfLeast(slopes);
  int midLine = (startLine + endLine)/2;
//  Serial.print(startLine);
//  Serial.print("    ");
//  Serial.println(endLine);
  
  //Serial.println();
  //Serial.println(millis());
  int e;
  int err;
  int sp;
  
  if ( endLine > startLine && endLine - startLine < ERROR_SIZE )
  {
    displayView(startLine, endLine, midLine);
    e = rawError(midLine);
    err = PID(midLine);
    sp = speedVal(e);
    analogWrite(motor1, sp);
    int angleDiff = map(err, -40, 40, -SERVO_RANGE, SERVO_RANGE);
    Serial.printf( " err: %d, ang: %d, sp: %d", err, angleDiff, sp);
    PREV_SAMPLES[curs] = angleDiff;
    curs = (curs + 1) % NUM_SAMPLES;
    ourServo.write(SERVO_CENTER - angleDiff);
    Serial.println();
  }
  else
  {
    //err = guessError();
    int angleDiffSum = 0;
    for ( int i = 0; i < NUM_SAMPLES; i++ )
    {
      angleDiffSum += PREV_SAMPLES[i];
    }
    int angleDiff = angleDiffSum / NUM_SAMPLES;
    ourServo.write(SERVO_CENTER - angleDiff);
    Serial.printf( "guessed error: %d", angleDiff);
    Serial.println();
  }
  //Serial.println(millis());
//  printValues();

  Serial.printf("\n\n");
  //delay(1000);
}

