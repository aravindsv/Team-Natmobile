#define MINSPEED 35
#define MAXSPEED 55

int speedVal( float error )
{
  if (error > 50) {
    return 0;
  }
  return constrain(MINSPEED + (MAXSPEED - MINSPEED)*(1-((error/40)*(error/40))), MINSPEED, MAXSPEED);
}
