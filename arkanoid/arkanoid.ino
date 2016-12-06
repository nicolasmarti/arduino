int rotary_analog_input = A0;
int rotary_interrupt_digital_input = 2;

int rotation_counter = 0;

void setup(){

  pinMode(rotary_analog_input, INPUT);

  rotation_counter = 0;
  
  //

  attachInterrupt( 0, rotate, FALLING);   
  
  Serial.begin(9600);

}

void rotate(){

    if (digitalRead( rotary_analog_input ) == HIGH)
      rotation_counter += 1;
    else
      rotation_counter -= 1;
      
}

void loop(){

  while (Serial.available() > 0) {
    
    char buffer[2];

    Serial.readBytesUntil('?', buffer, 1);
    
    noInterrupts();
    int ctr = rotation_counter;
    rotation_counter = 0;
    interrupts();

    Serial.println(ctr);


  }

}