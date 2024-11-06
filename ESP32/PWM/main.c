const int readPWMPin = 14; // Define the input pin
const int writePWMPin = 12; // Define the output pin
const int pinIN1 = 15; // Define the input pin
const int pinIN2 = 16; // Define the input pin

float readFrequency () {
  int read = analogRead(pinIN1);

  return map( read, 0, 4095, 10, 10000 );
}

float readDuty () {
  int read = analogRead(pinIN2);

  return map( read, 0, 4095, 0, 100 );
}

void setup() {
  Serial.begin(9600);
  Serial.println("================= Setup start =================");

  pinMode(pinIN1, INPUT);
  pinMode(pinIN2, INPUT);

  pinMode(readPWMPin, INPUT);
  pinMode(writePWMPin, OUTPUT);

  ledcAttach(readPWMPin, 0, 8);
  ledcAttach(writePWMPin, 1, 8);

  Serial.println("================= Setup end =================");
}

void loop() {
  // Write duty
  int frequency = readFrequency();
  ledcWriteTone(writePWMPin, frequency); // 1KHz to 70KHz. The default is 1KHz
  Serial.print("Frequency assigned: ");
  Serial.println(frequency);

  // Write duty
  int duty = readDuty();
  ledcWrite(writePWMPin, duty); // 0% to 100%
  Serial.print("Duty assigned: ");
  Serial.println(duty);

  // Read frequency of the PIN
  int readFrecuency = ledcReadFreq(readPWMPin);
  Serial.print("Frequency read: ");
  Serial.println(readFrecuency);

  // Read duty cycle of the PIN
  int readDuty = ledcRead(readPWMPin);
  Serial.print("Duty read: ");
  Serial.println(readDuty);

  delay(50);
}
