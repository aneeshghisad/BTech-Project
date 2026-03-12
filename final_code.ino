#include <OneWire.h>
#include <DallasTemperature.h>

/* ------------ TEMPERATURE (DS18B20) ------------ */
#define ONE_WIRE_BUS 2
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature tempSensor(&oneWire);

/* ------------ ODOR SENSOR ------------ */
#define ODOR_ANALOG A1
#define ODOR_DIGITAL 3
#define ODOR_LED 13

/* ------------ COLOR SENSOR (TCS3200) ------------ */
#define S0 5
#define S1 4
#define S2 7
#define S3 6
#define OUT 8

/* ------------ TURBIDITY ------------ */
#define TURBIDITY_PIN A2
const int CLEAR_THRESHOLD = 32;
const int NUM_SAMPLES = 10;

/* ------------ pH SENSOR ------------ */
#define PH_PIN A0
#define VREF 5.0
float calibration_value = 16.5176;

/* ================================================ */
void setup() {
  Serial.begin(9600);
  //Serial.println("Milk Quality Monitoring System");

  // Temperature
  tempSensor.begin();

  // Odor
  pinMode(ODOR_LED, OUTPUT);
  pinMode(ODOR_DIGITAL, INPUT);

  // Color sensor
  pinMode(S0, OUTPUT);
  pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT);
  pinMode(S3, OUTPUT);
  pinMode(OUT, INPUT);
  digitalWrite(S0, HIGH);
  digitalWrite(S1, LOW);

  // Turbidity
  pinMode(TURBIDITY_PIN, INPUT);
}

/* ================================================ */
void loop() {

  /* -------- TEMPERATURE -------- */
  tempSensor.requestTemperatures();
  float tempC = tempSensor.getTempCByIndex(0);
  // Serial.print("Temp (°C): ");
  // Serial.println(tempC);

  /* -------- ODOR -------- */
  int odorAnalog = analogRead(ODOR_ANALOG);
  int odorDigital = digitalRead(ODOR_DIGITAL);

  digitalWrite(ODOR_LED, odorAnalog > 300 ? HIGH : LOW);
  // Serial.print("Odor Analog: ");
  // Serial.print(odorAnalog);
  // Serial.print(" | Odor Digital: ");
  // Serial.println(odorDigital);

  /* -------- COLOR -------- */
  int red = readRed();
  int green = readGreen();
  int blue = readBlue();

  red   = constrain(map(red, 13, 228, 255, 0), 0, 255);
  green = constrain(map(green, 13, 255, 255, 0), 0, 255);
  blue  = constrain(map(blue, 13, 224, 255, 0), 0, 255);

  // Serial.print("RGB: ");
  // Serial.print(red);
  // Serial.print(", ");
  // Serial.print(green);
  // Serial.print(", ");
  // Serial.println(blue);

  /* -------- TURBIDITY -------- */
  long sum = 0;
  for (int i = 0; i < NUM_SAMPLES; i++) {
    sum += analogRead(TURBIDITY_PIN);
    delay(10);
  }

  int avgValue = sum / NUM_SAMPLES;
  int turbidityState;

  if (avgValue >= CLEAR_THRESHOLD) {
    turbidityState = 1;
  } else {
    turbidityState = 0;
  }
  //int turbidityState = digitalRead(TURBIDITY_PIN);
  // Serial.print("Turbidity: ");
  // Serial.println(turbidityState ? "CLEAR" : "CLOUDY");

  /* -------- pH -------- */
  int ph_adc = analogRead(PH_PIN);
  float ph_voltage = ph_adc * (VREF / 1023.0);
  float pH = -5.70 * ph_voltage + calibration_value;

  // Serial.print("pH: ");
  // Serial.println(pH, 2);

  // Serial.println("--------------------------------");
  delay(1000);

  // ---- SEND DATA TO PYTHON (CSV FORMAT) ----
  Serial.print(pH); Serial.print(",");
  Serial.print(tempC); Serial.print(",");
  Serial.print(odorDigital); Serial.print(",");
  Serial.print(turbidityState); Serial.print(",");
  Serial.println(red);

  delay(1000);   // 1 second sampling
}

/* ------------ COLOR FUNCTIONS ------------ */
int readRed() {
  digitalWrite(S2, LOW);
  digitalWrite(S3, LOW);
  return pulseIn(OUT, LOW);
}

int readGreen() {
  digitalWrite(S2, HIGH);
  digitalWrite(S3, HIGH);
  return pulseIn(OUT, LOW);
}

int readBlue() {
  digitalWrite(S2, LOW);
  digitalWrite(S3, HIGH);
  return pulseIn(OUT, LOW);
}
