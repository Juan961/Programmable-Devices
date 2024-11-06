const int minAngle = 0;
const int maxAngle = 180;
const int PWMPin = 12;
const int PWMFrequency = 50; // 50Hz
const int PWMPeriod = 1 / PWMFrequency; // 20ms
const int minValue = ( 1 / PWMPeriod ) * 100; // 1ms / 20ms: Porcentage mínimo para el servo 0 grados
const int maxValue = ( 2 / PWMPeriod ) * 100; // 2ms / 20ms: Porcentage máximo para el servo 180 grados
const int minValueMapped = map(minValue, 0, 100, 0, 255);
const int maxValueMapped = map(maxValue, 0, 100, 0, 255);
const int pwmChannel = 0; // Define the PWM channel

void move(int degrees) {
  if ( degrees > maxAngle || degrees < minAngle ) {
    Serial.println("Invalid servo range");
    return;
  }

  int duty = map(degrees, minAngle, maxAngle, minValueMapped, maxValueMapped);

  ledcWrite(PWMPin, duty);
}

void setup() {
  Serial.begin(9600);
  Serial.println("================= Setup start =================");

  pinMode(PWMPin, OUTPUT);
  ledcAttachPin(PWMPin, pwmChannel);

  ledcWriteTone(PWMPin, PWMFrequency);

  Serial.println("================= Setup end =================");
}

void loop() {
  int randNum = random(minAngle, maxAngle);
  Serial.print("Moving to: ");
  Serial.println(randNum);

  move(randNum);

  delay(300);
}
