const int THRESH = 15;

const double k_p = 5;
const double k_d = 0.5;
const double k_i = 0.001;
const int setpoint = 64;

const double dt = 1.5;
double prop, deriv, integ;
double error, prev_Error;
float decayFactor = 1.5;

// finds proportional. 
void P_controller() {

  prop = k_p * error;
}

void D_controller() {
  deriv = k_d * (error - prev_Error) / dt;
}

void I_controller() {
  static float integrator = 0;
  integrator += error; 
  integ = integrator * k_i;
  integrator /= decayFactor;
}

int rawError( int point )
{
  return point - setpoint;
}

int PID(int midpoint) {
  //Determine change in time; TODO look into this. currently const dt. 
  error = setpoint - midpoint;
  P_controller(); //Calculate error;
  //  TODO?: Calculate integral error;
  D_controller(); //  Calculate derivative error;
  //  TODO?: Apply gains to respective errors;
  //I_controller();
  prev_Error = error;   //  Save total error for next loop;
  error = prop + deriv + integ; //  Sum all errors;
  return error;
  //delay(dt);
}

//int guessError()
//{
////  deltaError = error - prev_Error;
////  prev_Error = error;
////  error += deltaError;
////  return error;
//  int sum = 0;
//  for ( int i = 0; i < NUM_SAMPLES; i++ )
//  {
//    sum += PREV_SAMPLES[i];
//  }
//  return sum / NUM_SAMPLES;
//}

void printValues()
{
  Serial.printf("error: %f, d_err: %f\n", error, deriv);
  if (error > THRESH)
  {
    Serial.println("Turn Left");
  }
  else if (error < -THRESH)
  {
    Serial.println("Turn Right");
  }
  else
  {
    Serial.println("Stay Straight");
  }
}
