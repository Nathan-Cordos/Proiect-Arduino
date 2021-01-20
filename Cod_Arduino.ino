#include <avr/interrupt.h>
#include<AFMotor.h>
#include<math.h>

AF_DCMotor m1(1);
AF_DCMotor m2(2);

// ultrasonic sensor control variables
long duration, distance;
double mapped_distance;
double avoid;


//position control variables
bool x_ok = true, y_ok = true,correct_y = false,correct_x = false;
bool finished = 0;
int w1 = 0, w2 = 0, dir_w1 = 1, dir_w2 = 1, x_max, x_min, y_max, y_min;
float c_a,L = 13.2, delta_w1, delta_w2, dist_w1, dist_w2, a = 0, x_new, y_new, x = 0, y = 0, dist_c = 0, a_new = 0;
volatile int w1_new = 0;
volatile  int w2_new = 0;

//neural net variables
double WeightsIH[3][5] = {{-9255963134931783073683178320070.000000, 1.808334 , 2.480664 ,0.130021, 3.850493},
                        {-9255963134931783073683178320070.000000, 8.666030, 2.541510 , -9.785462 ,-15.949171 }, 
                        {-9255963134931783073683178320070.000000 ,-12.373568 , -8.841073 , 9.732523 , 7.803736}};

double WeightsHO[5][4] = {
                      {-9255963134931783073683178320070.000000 , -0.213017 ,-6.880391 ,4.745813  },
                      {-9255963134931783073683178320070.000000 ,  12.185216 , -9.849756 , -4.594255},
                      {-9255963134931783073683178320070.000000 ,  3.545055 , -7.271459 , -6.668053  },
                      {-9255963134931783073683178320070.000000 ,-11.524251 , 6.865472 , 1.455714  },
                      {-9255963134931783073683178320070.000000 , -12.360523 , 13.873829 , -11.330289 }};
double H_OUT[5],OUT[4],INP[3];    
double cmp,ret,sum;


void setup() {
  pinMode(A2,INPUT);
  pinMode(A1,INPUT);
  pinMode(A5,OUTPUT);
  pinMode(A4,OUTPUT);
  cli();
  PCICR |= 0b00000011; // activate Analog and second batch of DI, intrerrupt vectors
  PCMSK1 |= 0b00000001;// A0
  PCMSK0 |= 0b00000010;// DI - 9
  sei();
  a = 0;
  x_min =-30;
  x_max =100;
  y_min =-30;
  y_max =100;
  INP[0] = 0;
 
 

}

void loop() {

// Ultrasonic sensor control
//------------------------------------------------------------------------------


digitalWrite(A4,HIGH);
delayMicroseconds(1000);
digitalWrite(A4, LOW);
duration=pulseIn(A2, HIGH);  
distance =(duration/2)/29.1;
mapped_distance = 10/distance;

if(mapped_distance > 1)mapped_distance = 1;
if(mapped_distance < 0.9) mapped_distance = 0;

INP[1] = mapped_distance;

digitalWrite(A5,HIGH);
delayMicroseconds(1000);
digitalWrite(A5, LOW);
duration=pulseIn(A1, HIGH);  
distance =(duration/2)/29.1;
mapped_distance = 10/distance;

INP[2] = mapped_distance;

avoid = forward_pass();

//------------------------------------------------------------------------------

//Position control


  //calculate new nr impulses
  delta_w1 = w1_new - w1;
  delta_w2 = w2_new - w2;

  //save nr of impulses
  w1 = w1_new;
  w2 = w2_new;

  //calculate distance traveled per wheel
  dist_w1 = 3.14*6.7*(delta_w1/40);
  dist_w2 = 3.14*6.7*(delta_w2/40);
  
  //calculate distance of center point of front axis of wheels
  dist_c =  (dist_w1+dist_w2)/2;

  //calculate angle
  a_new = a + (dist_w1-dist_w2)/L;

  //save angle
  a = a_new;

  if(a>6.28||a<-6.28) a = 0;
  
  //calculate new coordinates
  x_new = x+dist_c*cos(a);
  y_new = y+dist_c*sin(a);

  //save coordinates
  x = x_new;
  y = y_new;


  //determine if required x coordinate has been met
  if(!(x<x_max&&x>=x_min)) x_ok = false;
  
  //determine if required y coordinate has been met
  if(!(y<y_max&&y>=y_min)) y_ok = false;
  
if(!x_ok&&!y_ok) //determine if car is within bounds
  {finished = 1;
  stop_m();
  }  
else
{ 
  if(1 > avoid && avoid > 0.9) // if obstacle has been detected
  {
  // diferent actions based upon which output of the neural-net was activated  
  if(ret==1)avoid_left(); 
  if(ret==2)avoid_right();
  if(ret==3)go_back();
  }
  else
  {
    
    
    if(x_ok)// first the car goes to the required x coordinate
    {
      if(correct_x) //if the angle is correct for increasing x coordinate
      {
        forward();
      }
      else
      {
        correct_angle_x(); //if the angle is not correct
      }
    }

    if(!x_ok&&y_ok) // after the car goes to the required y coordinate
    {

      if(correct_y) //if the angle is correct for increasing y coordinate
      {
        forward();
      }
      else{
        correct_angle_y(); // if the angle is not correct
      }
     
    }

  }
    
   
  
}
    


}

ISR(PCINT0_vect) //intrerrupts which are triggered from position 
{
  if(dir_w1 == 1)w1_new++;

  else w1_new--;
 
 
}

ISR(PCINT1_vect)
{
  if(dir_w2 == 1) w2_new++;
  
  else w2_new--;
}


void stop_m()
{
  m1.setSpeed(0);
  m2.setSpeed(0);
}


void avoid_right()
{
  correct_x = false; //assume angle is wrong after avoiding
  correct_y = false; //assume angle is wrong after avoiding
  m1.setSpeed(0);
  m1.run(RELEASE);
  m2.run(RELEASE);
  m2.setSpeed(100);
  dir_w2 = 0;
  m2.run(FORWARD);
  
  delay(300);
  forward();
  delay(300);


}


void forward()
{
  m1.run(RELEASE);
  m2.run(RELEASE);
  dir_w1 = 1;
  dir_w2 = 1;
  m1.setSpeed(150);
  m2.setSpeed(150);
  m1.run(BACKWARD);
  m2.run(BACKWARD);
}

void go_back()
{
  m1.run(RELEASE);
  m2.run(RELEASE);
  dir_w1 = 0;
  dir_w2 = 0;
  m1.run(FORWARD);
  m2.run(FORWARD);
  m1.setSpeed(150);
  m2.setSpeed(150);
  delay(300);
}

void avoid_left()
{ correct_x = false; //assume angle is wrong after avoiding
  correct_y = false; //assume angle is wrong after avoiding
  m2.setSpeed(0);
  m1.run(RELEASE);
  m2.run(RELEASE);
  m1.setSpeed(100);
  dir_w1 = 0;
  m1.run(FORWARD);
  delay(300);
  forward();
  delay(300);
  
}



void correct_angle_x()
{

  m1.run(RELEASE);
  m2.run(RELEASE);
  
  m2.setSpeed(0);
  
  if(!((a/3.14<0.2)&&(a/3.14>-0.2)))
  {dir_w1 = 0;
   m1.setSpeed(100);
    m1.run(FORWARD);
    correct_x = false;
  }
  else
  {
    correct_x = true;
   
  }
  
}

void correct_angle_y()

{

  m2.setSpeed(0);
  
  if(!((a/3.14<0.5)&&(a/3.14>0.40)))
  {
    dir_w1 = 1;
    m1.setSpeed(100);
    m1.run(BACKWARD);
    correct_y = false;
  }
  else
  {
    correct_y = true;
    
  
  }

}

  double forward_pass() // propagate inputs through neural network
  {
    int i,j;
    cmp= 0;
    sum =0;
    ret = 0;
    for (j = 1; j <= 4; j++) {    
        sum = WeightsIH[0][j];
        for (i = 1; i <= 2; i++) {
          sum += INP[i] * WeightsIH[i][j];
        }
        H_OUT[j] = 1.0 / (1.0 + exp(-sum));
      }

      for (i = 1; i <= 3; i++) {  
        sum = WeightsHO[0][i];
        for (j = 1; j <= 4; j++) {
          sum += H_OUT[j] * WeightsHO[j][i];
        }
    OUT[i] = 1.0 / (1.0 + exp(-sum));
    
    }

    ret = 0;
    cmp = 0;
    for(i = 1;i<4;i++)
    {
    if(OUT[i]>cmp){ret =i;cmp = OUT[i];}//remember index
    }
    
  return cmp; //return highest value output
  }
 
