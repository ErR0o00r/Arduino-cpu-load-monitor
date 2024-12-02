int val = 0;

void LoadDisplay(int val) {
  if (val == 'L') {
    digitalWrite(3, HIGH);
    digitalWrite(5, LOW);
    digitalWrite(9, LOW);
  }
  if (val == 'M') {
    digitalWrite(3, HIGH);
    digitalWrite(5, HIGH);
    digitalWrite(9, LOW);
  }
  if (val == 'H') {
    digitalWrite(3, HIGH);
    digitalWrite(5, HIGH);
    digitalWrite(9, HIGH);
  }
}

void setup() {  // pins can be any. 3 5 and 9 were chosen for the convenience of layout on the breadboard
  Serial.begin(9600); 
  pinMode(13, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(9, OUTPUT);

}

void loop() {
  if (Serial.available() > 0) {
    val = Serial.read();
    LoadDisplay(val);
  }
}