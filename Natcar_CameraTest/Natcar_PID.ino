const double k_p = 1;
const double k_d = 0.5;
const int setpoint = 64;

const int dt = 10;
double prop, deriv;
double error, prev_Error;

// finds proportional. 
void P_controller() {

  prop = k_p * error;
}

void D_controller() {
  deriv = k_d * (error - prev_Error) / dt;
}

void PID(int midpoint) {
  //Determine change in time; TODO look into this. currently const dt. 
  error = setpoint - midpoint; 
  P_controller(); //Calculate error;
  //	TODO?: Calculate integral error;
  D_controller(); //	Calculate derivative error;
  //	TODO?: Apply gains to respective errors;
  error = prop + deriv; //	Sum all errors;
  prev_Error = error;   //	Save total error for next loop;
  delay(dt);
}

void printValues()
{
  printf("error: %d, d_err: %d\n", error, deriv);
}
