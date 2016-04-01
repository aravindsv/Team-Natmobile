//#include "Natcar_PID.ino"
#include <Servo.h>

// Camera Control Variables. 
#define LED 13
#define CLK 20       
#define AO 18        
#define SI 22

#define SERVO_PIN 10 //10 for Perfboard, 13 for PCB
#define SERVO_CENTER 85 // actually 90, but accounts for cam bias
#define SERVO_RANGE 15
#define SCALINGFACTOR 20
#define THRESHOLD 4
#define SLOPELENGTH 5 // this can be fine tuned in t he future. 
#define SKIPVAL 1

#define EDGE_IGNORE 0
#define ERROR_SIZE 15
#define MOTOR_POWER 35

int mini = 1000;
int maxi = 0;
int pixels[128];
int mapped[128];
int slopes[128];
float filter[128]; // TODO check if this has to be larger in any way. 
int smoothed_input[128]; // initialized to all 0s.
// consider using constants for the size, instead of 128, in the case that we want to reduce running time. 

// getting the input. 
// consider getting less than 128. This would significantly lower our time. 

// this is so that input goes from -64 to 63.
int scaled_index(int i)
{
  if ( i >= -64 && i <= 63 )
    return i + 64;
  return -1;
}

const int NUM_SAMPLES = 10;
int PREV_SAMPLES[NUM_SAMPLES];
int curs = 0;

Servo ourServo;

void setup()
{
  pinMode(SI, OUTPUT);
  pinMode(CLK, OUTPUT);
  pinMode(AO, INPUT);
  pinMode(LED, OUTPUT);
  
  ourServo.attach(SERVO_PIN);

  digitalWrite(SI, LOW);
  digitalWrite(CLK, LOW);

  Serial.begin(9600);

  digitalWrite(LED, HIGH);

  Gaussian_filter(2);
  
  delay(1000);
  
}

// simplistic filter that can be used as a test if gaussian doesn't work. 
void box_filter ( int sigma )
{
  for( int i = -64; i < 64; i++ )
  {
    if ( i < -sigma || i > sigma )
      filter[scaled_index(i)] = 0;
    else
      filter[scaled_index(i)] = 1.0 / (1 + 2 * sigma); 
  }
}
      
// intermediate filter that could work. mostly used as a test though. 
//void expo_filter ( int sigma ) // currently sigma doesn't actually do anything
//{
//  filter[scaled_index(0)] = 0.5;
//  
//  for i in (-64, 63):
//    int expo = i; 
//    if ( i < 0 )
//      expo = -i; //(get absolute value)
//    filter[scaled_index(i)] = (1/2)^expo;
//}

// notice that for any i > 3*sigma, the value may be completely negligible. 
// we can possibly save on time, in that sense. 
void Gaussian_filter ( int sigma )
{
  for( int i = -64; i < 64; i++ )
  {
    filter[scaled_index(i)] = exp((-i*i/(2*sigma)))/(sigma*sqrt(2*3.14159));
  }
}

// essentially does a convolution
// returns an array that has undergone a lowpass filter. 
void smoothing()
{
  for ( int i = -64; i < 63; i++ )
  {
    smoothed_input[scaled_index(i)] = 0;
  }
  
  for( int i = -64; i < 63; i++ )
  {
    for( int k = -64; k < 63; k++ )
    {
      smoothed_input[scaled_index(i)] += pixels[scaled_index(k)] * filter[scaled_index(i-k)];
    }
  }        
}

void calcSecant(int pixels[])
{
  for (int i = 0; i < 127 - SLOPELENGTH; i++)
  {
    slopes[i] = smoothed_input[i+SLOPELENGTH] - smoothed_input[i];
  }
  for( int i = 0; i < SLOPELENGTH; i++ )
  {
    slopes[127 - SLOPELENGTH + i] = 0;
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
  int greatest = 0;
  for (int i = 0; i < 128; i++)
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
  int least = 0;
  for ( int i = 0; i < 128; i++ )
  {
    if ( slopes[i] < slopes[least] )
    {
      least = i;
    }
  }
  return least;
}

//int indexOfLeast(int array[])
//{
//  int least = 128-SLOPELENGTH-EDGE_IGNORE-1;
//  for (int i = 128-SLOPELENGTH-EDGE_IGNORE-1; i >= EDGE_IGNORE; i--)
//  {
//    if (slopes[i] < slopes[least])
//    {
//      least = i;
//    }
//  }
//  return least;
//}

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

  for (int i = 0; i < 128; i++ )
  {
    digitalWrite(CLK, LOW);
    pixels[i] = analogRead(AO);
    pixels[i] -= 400;
    if ( pixels[i] < 0 )
      pixels[i] = 0;
    digitalWrite(CLK, HIGH);
  }
  digitalWrite(CLK, LOW);
  
//  for (int i = 0; i < 128 / SKIPVAL; i++)
//  {
//    if (pixels[i] > maxi)
//      maxi = pixels[i];
//    if (pixels[i] < mini)
//      mini = pixels[i];
//  }
//  for (int i = 1; i < 128; i++)
//  {
//    Serial.print(map(pixels[i], 0, 128, 0, 9));
//    Serial.print("");
//    //Serial.printf("%d ", pixels[i] / 20 );
//  }
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
  calcSecant(pixels);
  int startLine = indexOfGreatest(slopes);
  int endLine = indexOfLeast(slopes);
  int midLine = (startLine + endLine)/2;

  smoothing();  

  //Serial.println();
//  for ( int i = 0; i < 128; i++ )
//  {
//    Serial.print(pixels[i]);
//    Serial.print(" ");
//  }
//  Serial.println();

  for ( int i = 0; i < 128; i++ )
  {
    mapped[i] = smoothed_input[i] / 25;
  }
  for ( int i = 40; i >= 0; i-- )
  {
    for ( int j = 0; j < 128; j++ )
    {
      if ( mapped[j] >= i )
        Serial.print("*");
      else
        Serial.print(" ");
    }
    Serial.println();
  }

  displayView(startLine, endLine, midLine);

  
//  for ( int i = 0; i < 128; i++ )
//  {
//    Serial.print(mapped[i]);
//  }
  Serial.println();
  Serial.println();
  //Serial.println();
//  Serial.print(startLine);
//  Serial.print("    ");
//  Serial.println(endLine);
  
  //Serial.println();
  //Serial.println(millis());
  //int e;
  int err;
  //int sp;

  err = PID(midLine);
  int angleDiff = map(err, -40, 40, -SERVO_RANGE, SERVO_RANGE);
  ourServo.write(SERVO_CENTER + angleDiff);
  
//  if ( endLine > startLine && endLine - startLine < ERROR_SIZE )
//  {
//    displayView(startLine, endLine, midLine);
//    e = rawError(midLine);
//    err = PID(midLine);
//    sp = speedVal(e);
//    analogWrite(motor1, sp);
//    int angleDiff = map(err, -40, 40, -SERVO_RANGE, SERVO_RANGE);
//    Serial.printf( " err: %d, ang: %d, sp: %d", err, angleDiff, sp);
//    PREV_SAMPLES[curs] = angleDiff;
//    curs = (curs + 1) % NUM_SAMPLES;
//    ourServo.write(SERVO_CENTER - angleDiff);
//    Serial.println();
//  }
//  else
//  {
//    //err = guessError();
//    int angleDiffSum = 0;
//    for ( int i = 0; i < NUM_SAMPLES; i++ )
//    {
//      angleDiffSum += PREV_SAMPLES[i];
//    }
//    int angleDiff = angleDiffSum / NUM_SAMPLES;
//    ourServo.write(SERVO_CENTER - angleDiff);
//    Serial.printf( "guessed error: %d", angleDiff);
//    Serial.println();
//  }
  //Serial.println(millis());
//  printValues();

  //Serial.print("\n\n\n\n");
  //delay(1000);
}

