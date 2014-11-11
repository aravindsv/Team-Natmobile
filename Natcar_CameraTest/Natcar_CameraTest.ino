#define CLK 19
#define AO 23
#define SI 20
#define SCALINGFACTOR 20
#define THRESHOLD 4

int pixels[128];

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
//    delay(100);
    int disp;
    if (analogRead(AO)/SCALINGFACTOR <= THRESHOLD)
    {
      disp = 0;
    }
    else
    {
      disp = 1;
    }
    Serial.printf("%d", disp);
    digitalWrite(CLK, LOW);
//    delay(100);
  }
  
  Serial.printf("\n\n");
  //delay(1000);
}
