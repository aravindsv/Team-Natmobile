//#include "Natcar_PID.ino"

// Camera Control Variables. 
#define CLK 19
#define AO 23
#define SI 20
#define SCALINGFACTOR 20
#define THRESHOLD 4
#define SLOPELENGTH 10 // this can be fine tuned in the future. 

int pixels[128];
int slopes[128];

void setup()
{
  pinMode(SI, OUTPUT);
  pinMode(CLK, OUTPUT);
  pinMode(AO, INPUT);
  
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
    printf("%c", disp[i]);
  }
}

void loop()
{
  digitalWrite(SI, HIGH);
  digitalWrite(CLK, HIGH);
  digitalWrite(SI, LOW);
  digitalWrite(CLK, LOW);
  
  int i; 
  for ( i = 1; i < 128; i++ )
  {
    digitalWrite(CLK, HIGH);
    pixels[i] = analogRead(AO);
    digitalWrite(CLK, LOW);
  }
  calcSecant(pixels);

  Serial.println();
  int startLine = indexOfGreatest(slopes);
  int endLine = indexOfLeast(slopes);
  int midLine = (startLine + endLine)/2;
  //displayView(startLine, endLine, midLine);
  
  PID(midLine);
  printValues();
  
  printf("\n\n");
  //delay(1000);
}
