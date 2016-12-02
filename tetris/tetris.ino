

void setup(){

  pinMode( A0, 0 );

  Serial.begin(9600);
    
}

void loop(){

  while (Serial.available() > 0) {

    char buffer[2];

    Serial.readBytesUntil('?', buffer, 1);

    int value = analogRead( A0 );

    if (value == 0) {
      Serial.println("rotateleft");
      continue;
    }

    if (value < 550){
      Serial.println("rotateright");
      continue;
    }

    if (value < 700){
      Serial.println("shiftleft");
      continue;
    }

    if (value < 800){
      Serial.println("down");
      continue;
    }

    if (value < 1000){
      Serial.println("shiftright");
      continue;
    }

    Serial.println("none");

  }
    
}
