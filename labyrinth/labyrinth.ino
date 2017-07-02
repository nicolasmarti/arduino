#include <math.h>
#include <Wire.h>
#include "TM1637.h"
//#include <SPI.h>
#include <UTFT.h>

// this is for the Segment LED brick

#define CLK 12
#define DIO 13

TM1637 tm1637( CLK, DIO );

UTFT mlcd(ITDB18SP,4,3,7,6,5);//get lcd controler(model,data,clk,cs,rst,control)

void display_number(int a){
  
  //tm1637.clearDisplay();
  for (int i = 4; i != 0; --i){
    tm1637.display(i-1, a % 10);
    a = a /10;
  }
  
}


//
int button_analog_input = A0;
int buzzer_digital_output = 6;

///

#define DISPLAYX 0
#define DISPLAYY 1
#define DISPLAYZ 2

int state;

///


void compute_angles(float* angle_x, float* angle_y, float* angle_z);

void NineDoF_init();

////

#define LCD_BL  2
extern uint8_t SmallFont[];

////

int max_display_X;
int max_display_Y;


////

float pos_X;
float pos_Y;
float vel_X;
float vel_Y;
float accel_X;
float accel_Y;

int radius = 4;

#define pi 3.14159

unsigned long previous_time;
unsigned long time_now;

////

void setup(){

  unsigned long now = millis();
    
  pinMode(button_analog_input, INPUT_PULLUP);
  
  tm1637.init();
  tm1637.set( BRIGHT_TYPICAL );
  
  pinMode( buzzer_digital_output, OUTPUT );

  state = DISPLAYY;

  Serial.begin(9600);

  //Initialize the I2C communication. This will set the Arduino up as the 'Master' device.
  Wire.begin();

  NineDoF_init();

  pinMode(LCD_BL,OUTPUT);
  digitalWrite(LCD_BL,HIGH);
  mlcd.InitLCD(0);
  mlcd.setFont(SmallFont);
  mlcd.clrScr();  //refresh the lcd

  max_display_X = mlcd.getDisplayXSize();
  max_display_Y = mlcd.getDisplayYSize();

  pos_X = max_display_X / 2;
  pos_Y = max_display_Y / 2;

  previous_time = millis();

  Serial.print(cos(pi), 2);
  Serial.print('\t');
  
  mlcd.setFont(SmallFont);
  
}

void loop(){

  ////////////////////////
  
  time_now = millis();
  float dt = (time_now - previous_time) / 1000.0;
  previous_time = time_now;

  ////////////////////////

  float angleX, angleY, angleZ;

  compute_angles(&angleX, &angleY, &angleZ);

  ///

  accel_X = 9.8 * cos(pi / 2.0 - angleX * pi / 380.0 );
  accel_Y = 9.8 * cos(pi / 2.0 - angleY * pi / 380.0);
  
  vel_X = vel_X + accel_X * dt;
  vel_Y = vel_Y + accel_Y * dt;

  float old_pos_X = pos_X;
  float old_pos_Y = pos_Y;
  
  pos_X = min( max_display_X, max( 0.0,  pos_X + vel_X * dt) ); 
  pos_Y = min( max_display_Y, max( 0.0,  pos_Y + vel_Y * dt) );

  if (pos_X == old_pos_X)
    vel_X = 0;

  if (pos_Y == old_pos_Y)
    vel_Y = 0;

  ////////////////////////

  mlcd.setColor(255,0,0);
  
  /* mlcd.print("AngleX",10,10); // */
  /* mlcd.printNumI(angleX,10,26,4); */

  /* mlcd.print("AccelX",10,42); // */
  /* mlcd.printNumI(accel_X,10,58,4); */

  /* mlcd.print("VelX",10,74); // */
  /* mlcd.printNumI(vel_X,10,90,4); */

  /* mlcd.print("PosX",10,74+32); // */
  /* mlcd.printNumI(pos_X,10,90+32,4); */

  mlcd.printNumI(dt * 1000.0,10,74+32+32,4);

  
  if ( digitalRead( button_analog_input) == LOW ){

    if ( state == DISPLAYX )
      state = DISPLAYY;
    
    if ( state == DISPLAYY )
      state = DISPLAYZ;
    
    if ( state == DISPLAYZ )
      state = DISPLAYX;
    
  }

    
  mlcd.setColor(0,0,0);
  mlcd.drawCircle( old_pos_X, old_pos_Y, radius);

  mlcd.setColor(255,0,0);
  mlcd.drawCircle( pos_X, pos_Y, radius);
  

  
  //delay(10);

}

//////////////////////////////////////

// Mostly build from the code from elecfreacks


#define Acc (0x53)
#define Gyr (0x68)
#define Mag 0x1E

#define G_SMPLRT_DIV 0x15
#define G_DLPF_FS 0x16
#define G_INT_CFG 0x17
#define G_PWR_MGM 0x3E

int a_offx = -30;
int a_offy = -8;
int a_offz = 0;

int g_offx = 67;
int g_offy = 5;
int g_offz = 41;

#define Gry_offset -13
#define Gyr_Gain 0.07

unsigned long preTime;
float f_angleX;
float f_angleY;
float f_angleZ;

unsigned long lastTime;


void writeRegister(int device, byte address, byte val) {
  Wire.beginTransmission(device);
  Wire.write(address);
  Wire.write(val);
  Wire.endTransmission();
}

void readFrom(int DEVICE, byte address, int num, byte buff[]) {
  Wire.beginTransmission(DEVICE);
  Wire.write(address);
  Wire.endTransmission();
  Wire.beginTransmission(DEVICE);
  Wire.requestFrom(DEVICE, num);
  int i = 0;
  while(Wire.available()){
    buff[i] = Wire.read();
    i++;
  }
  Wire.endTransmission();
}
    
  
void NineDoF_init() {

  writeRegister(Acc, 0x2D, 0);
  writeRegister(Acc, 0x2D, 16);
  writeRegister(Acc, 0x2D, 8);  

  writeRegister(Gyr, G_PWR_MGM, 0x00);
  writeRegister(Gyr, G_SMPLRT_DIV, 0x07); // EB, 50, 80, 7F, DE, 23, 20, FF
  writeRegister(Gyr, G_DLPF_FS, 0x1E);    // +/- 2000 dgrs/sec, 1KHz, 1E, 19
  writeRegister(Gyr, G_INT_CFG, 0x00);

  preTime = 0;
  f_angleX = 0.0;
  f_angleY = 0.0;
  f_angleZ = 0.0;

    
}

void getGyroscopeData(int * result){

  int regAddress = 0x1B;
  int temp, x, y, z;
  byte buff[8];

  readFrom(Gyr, regAddress,8, buff);
  result[0] = ((buff[2] << 8) | buff[3]) + g_offx;
  result[1] = ((buff[4] << 8) | buff[5]) + g_offy;
  result[2] = ((buff[6] << 8) | buff[7]) + g_offz;
  result[3] = (buff[0] << 8) | buff[1];

}

void getAccelerometerData(int * result) {

  int regAddress = 0x32;
  byte buff[6];

  readFrom(Acc, regAddress, 6, buff);

  result[0] = (((int)buff[1]) << 8) | buff[0] + a_offx;
  result[1] = (((int)buff[3]) << 8) | buff[2] + a_offy;
  result[2] = (((int)buff[5]) << 8) | buff[4] + a_offz;

}

void compute_angles(float* angle_x, float* angle_y, float* angle_z){

  unsigned long now = millis();
  float dt = (now - preTime) / 1000.0;
  int time=now-preTime;
  preTime = now;

  int acc[3];
  int gyro[4];

  getAccelerometerData(acc);
  getAccelerometerData(gyro);

  float X_Acc = acc[0];
  float Y_Acc = acc[1];
  float Z_Acc = acc[2];

  float angleX = atan(Y_Acc / Z_Acc) * 180 / pi;
  float angleY = atan(X_Acc/Z_Acc  ) * 180 / pi;
  float angleZ = atan(X_Acc / Y_Acc) * 180 / pi;

  float omegaX =  Gyr_Gain * (gyro[0] +  Gry_offset);
  float omegaY =  Gyr_Gain * (gyro[1] +  Gry_offset);
  float omegaZ =  Gyr_Gain * (gyro[2] +  Gry_offset);

  float K = 0.8;
  float A = K / (K + dt);

  f_angleX =(A *(f_angleX + omegaX * dt) + (1-A) * angleX);
  f_angleY =(A *(f_angleY + omegaY * dt) + (1-A) * angleY);
  f_angleZ =(A *(f_angleZ + omegaZ * dt) + (1-A) * angleZ);

  *angle_x = f_angleX;
  *angle_y = f_angleY;
  *angle_z = f_angleZ;
  
}
