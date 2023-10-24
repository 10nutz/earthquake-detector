#define X_AXIS  0
#define Y_AXIS  1
#define Z_AXIS  2

double x_ref;
double y_ref;
double z_ref;




void setup() {
  Serial.begin(9600);

  x_ref = analogRead(X_AXIS);
  y_ref = analogRead(Y_AXIS);
  z_ref = analogRead(Z_AXIS);
}

void loop() {
  int x_new = analogRead(X_AXIS);
  int y_new = analogRead(Y_AXIS);
  int z_new = analogRead(Z_AXIS);


  Serial.print("X = ");
  Serial.println(xRead);

  Serial.print("Y = ");
  Serial.println(yRead);

  Serial.print("Z = ");
  Serial.println(zRead);

  Serial.println(" ");
  Serial.println(" ");
  Serial.println(" ");

  delay(4000);
}
