#include "TM1637.h"

#define CLK 4
#define DIO 5

TM1637 tm1637( CLK, DIO );


void display_number(int a){
  
  //tm1637.clearDisplay();
  for (int i = 4; i != 0; --i){
    tm1637.display(i-1, a % 10);
    a = a /10;
  }
  
}


void setup(){
 
  tm1637.init();
  tm1637.set( BRIGHT_TYPICAL );

  pinMode( A0, 0 );
 
  display_number(0);
  
}

void loop(){
  
  int value = analogRead( A0 );
  
  display_number( value );
  
 delay( 500 );
 
  
}

