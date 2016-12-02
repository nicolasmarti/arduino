#include "TM1637.h"

#define CLK 4
#define DIO 5

#define STOPPED 0
#define RUNNING 1
#define PAUSED 2
#define BUZZING 3

int current_state;

unsigned long remaining_time;
unsigned long start_time;

int led_output = 13;

int button_analog_input = A0;
int rotary_analog_input = A1;
int rotary_interrupt_digital_input = 2;
int buzzer_digital_output = 6;

TM1637 tm1637( CLK, DIO );

int led_status = LOW;

void display_time(unsigned long a){
  
  int minutes = a / 60;
  int seconds = a % 60;
  
  //tm1637.clearDisplay();
  tm1637.display(0, minutes / 10);
  tm1637.display(1, minutes % 10);
  tm1637.display(2, seconds / 10);
  tm1637.display(3, seconds % 10);
  
}



void setup(){
  
  pinMode(button_analog_input, INPUT_PULLUP);
  
  
  tm1637.init();
  tm1637.set( BRIGHT_TYPICAL );
  
  pinMode( buzzer_digital_output, OUTPUT );
  
  pinMode( led_output, OUTPUT );  
    
  pinMode(rotary_analog_input, INPUT);
  
  //

  attachInterrupt( 0, rotate, FALLING);   

  current_state = STOPPED;
  remaining_time = 0;

  display_time(remaining_time / 1000);  
  
}


void rotate(){
  
  if ( current_state == STOPPED ) {
    
    led_status = led_status == HIGH ? LOW : HIGH;
  
    digitalWrite( led_output, led_status );
  
    if (digitalRead( rotary_analog_input ) == HIGH)
      remaining_time = remaining_time < (99 * 60000 + 50000) ?  remaining_time + 10000 : 0;
    else
      remaining_time = remaining_time > 0 ?  remaining_time - 10000 : 0;
    
  }
  
}


void loop(){
 
  display_time(remaining_time / 1000); 
  
  switch (current_state) {
    
    case STOPPED:{

      if ( remaining_time > 0 && digitalRead( button_analog_input) == LOW ){
       
        delay( 500 );
        start_time = millis();
        current_state = RUNNING;
        
        
      }
     
      break; 
    }
    
    case RUNNING:{
      
      unsigned long now = millis();
      unsigned long elapsed = (now - start_time);

      if (elapsed > remaining_time){
        
        remaining_time = 0;
        analogWrite( buzzer_digital_output, 100 );           
        current_state = BUZZING;
        
      } else {
        
        remaining_time -= elapsed;
        start_time = now;
        
      }
 
      if ( digitalRead( button_analog_input) == LOW ){
       
        delay( 500 );
        current_state = PAUSED;
        
      }
 
     
      break; 
    }    
    
    case PAUSED:{

      if ( digitalRead( button_analog_input) == LOW ){
       
        delay( 500 );
        current_state = RUNNING;
        unsigned long now = millis();
        start_time = now;
      }
     
      break; 
    }    
    
    case BUZZING:{
    
      if ( digitalRead( button_analog_input) == LOW ){
       
        analogWrite( buzzer_digital_output, 0 );   
        delay( 500 );
        current_state = STOPPED;
        
      }
      break; 
    }
    
  }   
  
}
