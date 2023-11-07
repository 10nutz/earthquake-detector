#include <LiquidCrystal.h>

LiquidCrystal lcd(13, 12, 11, 6, 5, 4, 3);

#define X_AXIS A0
#define Y_AXIS A1
#define Z_AXIS A2

#define LED_ON A3
#define LED_EMERGENCY A4

#define BUZZER 2

#define BUTTON_CALIBRATION 8
#define BUTTON_RESET 9

#define samples 50
#define threshold 50

double x_ref;
double y_ref;
double z_ref;
bool emergencyActive = false;

void reset(){
  lcd.clear();
  lcd.print("OK!");
  digitalWrite(LED_EMERGENCY, LOW);
  digitalWrite(LED_ON, HIGH);
  noTone(BUZZER);
  emergencyActive = false;
}

void calibrate(){
  lcd.clear();
  lcd.print("Calibrating.....");
  delay(1000);
  lcd.clear();
  lcd.print("Please wait...");
  delay(1000);
    
  x_ref = 0;
  y_ref = 0;
  z_ref = 0;
  for (int i = 0; i < samples; i++) {
    x_ref += analogRead(X_AXIS);
    y_ref += analogRead(Y_AXIS);
    z_ref += analogRead(Z_AXIS);
    }
  x_ref /= samples;
  y_ref /= samples;
  z_ref /= samples;

  lcd.clear();
  lcd.print("Calibrated");
  delay(1000);
  lcd.clear();
  lcd.print("OK!");
}

void setup() {
  Serial.begin(9600);
  lcd.begin(16, 2);
  pinMode(LED_ON, OUTPUT);
  pinMode(LED_EMERGENCY, OUTPUT);
  pinMode(BUZZER, OUTPUT);
  pinMode(BUTTON_CALIBRATION, INPUT);
  pinMode(BUTTON_RESET, INPUT);
  
  digitalWrite(LED_ON, HIGH);


  delay(1000);
  lcd.print("EarthQuake ");
  lcd.setCursor(0, 1);
  lcd.print("Detector ");
  delay(2000);

  calibrate();
  lcd.clear();
  lcd.print("OK!");
}

void loop() {

  int x_new = analogRead(X_AXIS);
  int y_new = analogRead(Y_AXIS);
  int z_new = analogRead(Z_AXIS);

  if (abs(x_new - x_ref) > threshold || abs(y_new - y_ref) > threshold || abs(z_new - z_ref) > threshold) {
    lcd.clear();
    lcd.print("EMERGENCY!");
    digitalWrite(LED_ON, LOW);
    digitalWrite(LED_EMERGENCY, HIGH);
    emergencyActive = true;
    tone(BUZZER, 1000);
  }

  if (digitalRead(BUTTON_RESET) == HIGH && emergencyActive) {
    reset();
  }

  if (digitalRead(BUTTON_CALIBRATION) == HIGH) {
    reset();
    calibrate();
  }
}
