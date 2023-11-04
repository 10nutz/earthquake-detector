#include <LiquidCrystal.h>

LiquidCrystal lcd(13, 12, 11, 6, 5, 4, 3);

#define X_AXIS  A0
#define Y_AXIS  A1
#define Z_AXIS  A2

#define LED_ON A3
#define LED_EMERGENCY A4

#define BUZZER 2

#define BUTTON_CALIBRATION 8
#define BUTTON_RESET 9

#define samples 50
#define threshold 10

double x_ref;
double y_ref;
double z_ref;



void setup() {
  Serial.begin(9600);

  lcd.begin(16, 2);
  delay(1000);
  lcd.print("EarthQuake ");
  lcd.setCursor(0,1);
  lcd.print("Detector ");
  delay(2000);
  lcd.clear();
  lcd.print("Calibrating.....");
  lcd.setCursor(0,1);
  lcd.print("Please wait...");

  pinMode(LED_ON, OUTPUT);
  pinMode(LED_EMERGENCY, OUTPUT);
  pinMode(BUZZER, OUTPUT);

  
  pinMode(BUTTON_CALIBRATION, INPUT);
  pinMode(BUTTON_RESET, INPUT);

  delay(3000);
  digitalWrite(LED_ON, HIGH);

  for(int i=0;i<samples;i++)
  {
    x_ref += analogRead(X_AXIS);
    y_ref += analogRead(Y_AXIS);
    z_ref += analogRead(Z_AXIS);
  }

  x_ref /= samples;
  y_ref /= samples;
  z_ref /= samples;

  delay(3000);
  lcd.clear();
  lcd.print("Calibrated");
  delay(1000);
  lcd.clear();
  lcd.print("Device Ready");
}

void loop() {

  lcd.clear();
  lcd.print("OK!");

  int x_new = analogRead(X_AXIS);
  int y_new = analogRead(Y_AXIS);
  int z_new = analogRead(Z_AXIS);

  if(abs(x_new-x_ref) > threshold || abs(y_new-y_ref) > threshold || abs(z_new-z_ref) > threshold){
    lcd.clear();
    lcd.print("EMERGENCY!");
    digitalWrite(LED_ON, LOW);
    bool buton = digitalRead(BUTTON_RESET);
    while(buton != HIGH){
      buton = digitalRead(BUTTON_RESET);
      Serial.println(buton);
      digitalWrite(LED_EMERGENCY, HIGH);
      tone(BUZZER, 1000);
      delay(2000);
      digitalWrite(LED_EMERGENCY, LOW);
      noTone(BUZZER);
      delay(2000);
    }
  }

  if(BUTTON_RESET == HIGH){
    lcd.clear();
    lcd.print("OK!");
    digitalWrite(LED_ON, HIGH);
    digitalWrite(LED_EMERGENCY, LOW);
    noTone(BUZZER);
  }


  delay(4000);
}
