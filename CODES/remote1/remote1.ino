  int V1y = 35;
  int V1x = 34;
  int V2y = 12;
  int V2x = 13;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
}

void loop() {
  // put your main code here, to run repeatedly:
  int v1y= analogRead(V1y);
  int v1x = analogRead(V1x);

  int v2y= analogRead(V2y);
  int v2x = analogRead(V2x);

  delay(300);
  Serial.println("Joystick 1: ");
  Serial.print("Y: ");
  Serial.println(v1y); // 1770
  Serial.print("X: ");
  Serial.println(v1x); // 1820

  Serial.println("Joystick 2: ");
  Serial.print("Y: ");
  Serial.println(v2y); // 1860
  Serial.print("X: ");
  Serial.println(v2x); // 1860

}
