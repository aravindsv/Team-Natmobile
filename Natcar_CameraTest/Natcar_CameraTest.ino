//#include "Natcar_PID.ino"
#include <Servo.h>

//Motor Control variables
#define motor1 3     //3 for PCB
#define motor2 4     //4 for PCB

// Camera Control Variables. 
#define CLK 16       //2 for Perfboard, 16 for PCB
#define AO 14        //18 for Perfboard, 14 for PCB
#define SI 15        //3 for Perfboard, 15 for PCB
#define SERVO_PIN 13 //8 for Perfboard, 13 for PCB

#define SERVO_CENTER 125
#define SCALINGFACTOR 20
#define THRESHOLD 4
#define SLOPELENGTH 10 // this can be fine tuned in the future. 
#define SKIPVAL 1

int mini = 1000;
int maxi = 0;
int pixels[128 / SKIPVAL];
int slopes[128 / SKIPVAL];
Servo ourServo;

void setup()
{
  pinMode(SI, OUTPUT);
  pinMode(CLK, OUTPUT);
  pinMode(AO, INPUT);
  
  ourServo.attach(SERVO_PIN);

  digitalWrite(SI, LOW);
  digitalWrite(CLK, LOW);

  Serial.begin(9600);
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
  int greatest = 1;
  for (int i = 1; i < 128-SLOPELENGTH; i++)
  {
    if (array[i] > array[greatest])
    {
      greatest = i;
    }
  }
  return greatest;
}

int indexOfLeast(int array[])
{
  int least = 1;
  for (int i = 1; i < 128-SLOPELENGTH; i++)
  {
    if (array[i] < array[least])
    {
      least = i;
    }
  }
  return least;
}

void displayView(int startIndex, int endIndex, int midIndex)
{
  char disp[128];
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

  for (int i = 1; i < 128 / SKIPVAL; i++ )
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
  for (int i = 2; i < 128; i++)
  {
    Serial.print(map(pixels[i], 0, 128, 0, 9));
    Serial.print("");
    //Serial.printf("%d ", pixels[i] / 20 );
  }
  // Serial.printf("%d\t%d\n", mini, maxi );
  Serial.println("");
  
//  for (i = 0; i < 128; i++)
//  {
//    Serial.println(pixels[i]);
//  }
//  while(1)
//  {
//    delay(1000);
//  }
  //calcSecant(pixels);

  Serial.println();
  int startLine = indexOfGreatest(slopes);
  int endLine = indexOfLeast(slopes);
  int midLine = (startLine + endLine)/2;
//  Serial.print(startLine);
//  Serial.print("    ");
//  Serial.println(endLine);
//  displayView(startLine, endLine, midLine);
  Serial.println();
  //Serial.println(millis());
  // setServo(PID(midLine));
  //Serial.println(millis());
//  printValues();

  Serial.printf("\n\n");
  //delay(1000);
}

