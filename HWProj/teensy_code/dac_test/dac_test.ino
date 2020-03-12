int i, u;

void setup() {
  // put your setup code here, to run once:
  i = 0;
  u = 4095;
  Serial.begin(9600);
  analogWriteResolution(12);
  pinMode(13, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  analogWrite(A21, i);
  analogWrite(A22, u);
  i += 10;
  u -= 10;
  delay(50);
  Serial.println(i);
  if (i > 4095) {
    i = 0;
  }
  if (u < 0) {
    u = 4095;
  }
  digitalWrite(13, HIGH);
}
