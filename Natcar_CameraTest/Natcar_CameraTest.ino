#define CLK 19
#define AO 23
#define SI 20
#define SCALINGFACTOR 20
#define THRESHOLD 50 // TODO: change this value accordingly. 

#define MAXPIXELS 128
#define SLOPELENGTH 4 // this can be fine tuned in the future.
#define SKIPVAL 2 // this is the number of values we skip. 

int pixels[MAXPIXELS / SKIPVAL];
int slopes[MAXPIXELS / SKIPVAL - SLOPELENGTH]; 

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
  for (int i = 0; i < MAXPIXELS / SKIPVAL - SLOPELENGTH; i++)
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

// edite 11/22: this is untested, but these changes might be there. 
// we still should have some issues concerning finding the line at the edges though. just in case. 
int indexOfGreatest(int array[])
{
  int greatest = 1;
  for (int i = 1; i < MAXPIXELS / SKIPVAL - SLOPELENGTH - 1; i++)
  {
    if (array[i] > array[greatest])
    {
      greatest = i;
    }
  }
  
  for (int i = greatest; i < MAXPIXELS / SKIPVAL - SLOPELENGTH - 1; i++)
  {
    if ( slopes[i+1] - slopes[i] < THRESHOLD )
    {
      return SKIPVAL * greatest - SKIPVAL / 2;
    }
  }
  return -1; // this means that. the maximum value is never found and should never really come here. maybe it means that the line is at the edge of vision?
}

int indexOfLeast(int array[])
{
  int least = 0;
  for (int i = 0; i < MAXPIXELS / SKIPVAL - SLOPELENGTH - 1; i++)
  {
    if (array[i] < array[least])
    {
      least = i;
    }
  }
  
  for (int i = least; i >= 0; i--)
  {
    if ( slopes[i+1] - slopes[i] < THRESHOLD )
    {
      return SKIPVAL * least - SKIPVAL / 2;
    }
  }
  return -1; // this means that. the minimum value is never found and should never really come here. maybe it means that the line is at the edge of vision?
}

void displayView(int startIndex, int endIndex, int midIndex)
{
  char disp[MAXPIXELS];
  for (int i = 0; i < MAXPIXELS; i++)
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
    Serial.print(disp[i]);
  }
}

void loop()
{
  digitalWrite(SI, HIGH);
  digitalWrite(CLK, HIGH);
  digitalWrite(SI, LOW);
  digitalWrite(CLK, LOW);
  
  int i; 
  for ( i = 0; i < MAXPIXELS/ SKIPVAL; i += SKIPVAL )
  {
    digitalWrite(CLK, HIGH);
//    delay(100);
    pixels[i] = analogRead(AO);
    digitalWrite(CLK, LOW);
//    delay(100);
  }
  calcSecant(pixels);

  Serial.println();
  int startLine = indexOfGreatest(slopes);
  int endLine = indexOfLeast(slopes);
  int midLine = (startLine + endLine)/2;
//  Serial.printf("%d      %d       %d", startLine, midLine, endLine);
  displayView(startLine, endLine, midLine);
  
  Serial.print("\n\n");
  //delay(1000);
}
