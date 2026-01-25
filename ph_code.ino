// Simple pH Reader (P0 → A0)
// No LCD, no filtering, raw + voltage + pH

const int PH_PIN = A0;          // P0 from pH module
const float VREF = 5.0;         // Arduino reference voltage
float calibration_value = 16.5176; // Your calibration constant

void setup() {
  Serial.begin(9600);
  Serial.println("Simple pH Reader");
}

void loop() {

  int ph_adc = analogRead(PH_PIN);            // raw ADC 0–1023
  float ph_voltage = ph_adc * (VREF / 1023.0); // convert to volts

  // pH equation (your module uses this linear formula)
  float pH = -5.70 * ph_voltage + calibration_value;

  // Print
  Serial.print("ADC = ");
  Serial.print(ph_adc);

  Serial.print("   |   Voltage = ");
  Serial.print(ph_voltage, 3);
  Serial.print(" V");

  Serial.print("   |   pH = ");
  Serial.println(pH, 2);

  delay(1000);
}
