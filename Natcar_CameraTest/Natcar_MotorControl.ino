#define MINSPEED 35
#define MAXSPEED 65

int speedVal( int error )
{
  int absError = error;
  if (absError < 0)
    absError *= -1;
    
  return map(absError, 40, 0, MINSPEED, MAXSPEED);
}
