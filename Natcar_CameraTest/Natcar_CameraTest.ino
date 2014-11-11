#define CLK 19
#define AO 23
#define SI 20
#define SCALINGFACTOR 20
#define THRESHOLD 4

int pixels[128]; // array of pixels. 

void setup()
{
  pinMode(SI, OUTPUT);
  pinMode(CLK, OUTPUT);
  pinMode(AO, INPUT);
  
  digitalWrite(SI, LOW);
  digitalWrite(CLK, LOW);
  
  Serial.begin(9600);
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
    char disp;
    if (analogRead(AO)/SCALINGFACTOR <= THRESHOLD)
    {
      disp = ' ';
    }
    else
    {
      disp = '*';
    }
    Serial.printf("%c", disp);
    digitalWrite(CLK, LOW);
  }
  
  Serial.printf("\n\n");
}
