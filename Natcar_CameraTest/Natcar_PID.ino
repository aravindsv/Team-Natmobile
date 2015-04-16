const int THRESH = 15;

const double k_p = 1;
const double k_d = 0.5;
const int setpoint = 64;

const double dt = 1.5;
double prop, deriv;
double error, prev_Error = 0;

// finds proportional. 
void P_controller() {

  prop = k_p * error;
}

void D_controller() {
  deriv = k_d * (error - prev_Error) / dt;
}

int PID(int midpoint) {
  //Determine change in time; TODO look into this. currently const dt. 
  error = setpoint - midpoint; 
  P_controller(); //Calculate error;
  //	TODO?: Calculate integral error;
  D_controller(); //	Calculate derivative error;
  //	TODO?: Apply gains to respective errors;
  prev_Error = error;   //	Save total error for next loop;
  error = prop + deriv; //	Sum all errors;
  return error;
  //delay(dt);
}

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
