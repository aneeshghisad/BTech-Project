/*
 * TCS3200 / TCS230 Color Sensor
 * Reads RAW RGB values of an object
 */

#define S0 5
#define S1 4
#define S2 7
#define S3 6
#define OUT 8

void setup() {
  pinMode(S0, OUTPUT);
  pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT);
  pinMode(S3, OUTPUT);
  pinMode(OUT, INPUT);

  // Set frequency scaling to 20%
  digitalWrite(S0, HIGH);
  digitalWrite(S1, LOW);

  Serial.begin(9600);
}

void loop() {
  int redValue,greenValue,blueValue;
  
  redValue   = readRed();
  delay(100);
  greenValue = readGreen();
  delay(100);
  blueValue  = readBlue();
  delay(100);

  redValue = constrain(map(redValue, 13, 228, 255, 0), 0, 255);
  greenValue = constrain(map(greenValue, 13, 255, 255, 0), 0, 255);
  blueValue = constrain(map(blueValue, 13, 224, 255, 0), 0, 255);


  Serial.print("R = ");
  Serial.print(redValue);
  Serial.print(" | G = ");
  Serial.print(greenValue);
  Serial.print(" | B = ");
  Serial.println(blueValue);

  delay(500);
}

// ---------- Sensor Reading Functions ----------

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
