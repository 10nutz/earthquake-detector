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
volatile bool buzzerState = false;

void reset() {
  lcd.clear();
  lcd.print("OK!");
  PORTC |= (1 << 3);
  PORTC &= ~(1 << 4);
  buzzerState = false;
  emergencyActive = false;
}

void calibrate() {
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
  PORTC = 0x00; // initializare port
  DDRC |= 0x18; // setam ca iesiri porturile 3 si 4 pt led

  TCCR1A = 0x00; // timer pentru buzzer
  TCCR1B = 0x00;
  TCCR1A = 0x40;
  TCCR1B = 0x0A;  // Set WGM12 and CS11 bits for CTC mode and prescaler of 8
  OCR1A = 8000;

  TIMSK1 |= (1 << 1);  // Enable Timer 1 Output Compare A Match interrupt

  PORTD = 0x00; // Buzzer + lcd
  DDRD |= 0x3C;

  PORTB = 0x00; // LCD + butoane
  DDRB |= 0x7C;

  PORTC = (1 << 3);

  delay(1000);
  lcd.print("EarthQuake ");
  lcd.setCursor(0, 1);
  lcd.print("Detector ");
  delay(2000);

  calibrate();
  lcd.clear();
  lcd.print("OK!");
}

ISR(TIMER1_COMPA_vect) {
  if (buzzerState) {
    PORTD ^= (1 << 2); // Toggle the buzzer pin state
  } else {
    PORTD &= ~(1 << 2); // Turn off the buzzer
  }
}

void loop() {
  int x_new = analogRead(X_AXIS);
  int y_new = analogRead(Y_AXIS);
  int z_new = analogRead(Z_AXIS);

  if ((abs(x_new - x_ref) > threshold || abs(y_new - y_ref) > threshold || abs(z_new - z_ref) > threshold) && emergencyActive == false) {
    lcd.clear();
    lcd.print("EMERGENCY!");
    PORTC &= ~(1 << 3);
    PORTC |= (1 << 4);
    emergencyActive = true;
    buzzerState = true;
  }

  if (digitalRead(BUTTON_RESET) == HIGH && emergencyActive == true) {
    reset();
  }

  if (digitalRead(BUTTON_CALIBRATION) == HIGH) {
    reset();
    calibrate();
  }
}
