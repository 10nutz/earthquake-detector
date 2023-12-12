#include <LiquidCrystal.h>

LiquidCrystal lcd(13, 12, 11, 6, 5, 4, 3);

#define X_AXIS A0
#define Y_AXIS A1
#define Z_AXIS A2
#define BC 8
#define samples 50
#define threshold 10  // momentan e mic ca sa facem debug mai usor

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
    x_ref += analogReadCustom(X_AXIS);
    y_ref += analogReadCustom(Y_AXIS);
    z_ref += analogReadCustom(Z_AXIS);
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
  Serial.println("test2");
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

  PORTB = 0x00; // LCD
  DDRB |= 0x7C;

  PORTC = (1 << 3);

  ADMUX = 0x00; //initialize the register
  ADMUX |= (1<<REFS0); // (1<<6) the Vcc (5V theoretically is used as Vref)
  ADCSRA |= 0b00000111; // set prescaler to 128
  ADCSRA |= (1<<ADEN); //enable ADC operation

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

int analogReadCustom(int pin) {
  // Set the ADC channel
  ADMUX = (ADMUX & 0xF0) | (pin & 0x0F);

  // Start ADC conversion
  ADCSRA |= (1 << ADSC);

  // Wait for conversion to complete
  while (ADCSRA & (1 << ADSC));

  // Return the ADC result
  return ADC;
}

void loop() {
int channel = 0;
  int x_new = analogReadCustom(X_AXIS);
  int y_new = analogReadCustom(Y_AXIS);
  int z_new = analogReadCustom(Z_AXIS);


  if ((abs(x_new - x_ref) > threshold || abs(y_new - y_ref) > threshold || abs(z_new - z_ref) > threshold) && emergencyActive == false) {
    lcd.clear();
    lcd.print("EMERGENCY!");
    PORTC &= ~(1 << 3);
    PORTC |= (1 << 4);
    emergencyActive = true;
    buzzerState = true;
  }

  int buttonStateReset = (PINC & (1 << 5)) ? HIGH : LOW;
  if (buttonStateReset == HIGH && emergencyActive == true) {
    reset();
  }

  int buttonStateCalibration = (PINB & (1 << 0)) ? HIGH : LOW;
  if (buttonStateCalibration == HIGH) {
    reset();
    calibrate();
  }
}
