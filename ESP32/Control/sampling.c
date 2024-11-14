#include <BluetoothSerial.h> // Bluetooth Serial communication on ESP32
#define PI 3.1415926535897932384626433832795

// Check if Bluetooth is available
#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

// Check Serial Port Profile
#if !defined(CONFIG_BT_SPP_ENABLED)
#error Serial Port Profile for Bluetooth is not available or not enabled. It is only available for the ESP32 chip.
#endif

String device_name = "JJBlue";
BluetoothSerial SerialBT;
hw_timer_t *timer = NULL;

volatile float duty = 110;
// volatile float angular_velocity = 0;
const int MOTOR_PIN_1 = 25;
const int MOTOR_PIN_2 = 33;
const int MOTOR_PIN_PWM = 32;
const int ENCODER_PIN = 4;
const int PWM_FREQUENCY = 5000;  // Frequency in Hz
const int PWM_RESOLUTION = 8;    // Resolution in bits (from 1 to 15)

const float encoder_radius = 0.017; // 1.7cm
const float encoder_perimeter = 2 * encoder_radius * PI;
const int total_holes = 20;
const float distance_between_holes = encoder_perimeter / total_holes;
volatile unsigned long last_rising = 0;  // last timestamp in milliseconds
volatile unsigned long time_elapsed = 0;

bool start = false;
unsigned long start_time = 0;

void IRAM_ATTR encoderInterrupt () {
  unsigned long now_micros = millis();
  time_elapsed = now_micros - last_rising;
  last_rising = now_micros;
}

void ARDUINO_ISR_ATTR timerInterrupt() {
  float angular_velocity = ( (distance_between_holes / ( time_elapsed / 1000.0 )) / encoder_radius ) * (60 / (2 * PI));

  Serial.print(millis() - start_time);
  Serial.print(";");
  Serial.println(angular_velocity);
}

void bluetoothCallback(esp_spp_cb_event_t event, esp_spp_cb_param_t *param ) {
  // if (event == ESP_SPP_START_EVT) {} // Bluetooth Serial Port Profile initialized
  // else if (event == ESP_SPP_SRV_OPEN_EVT ) {} // Bluetooth client connected
  // else if (event == ESP_SPP_CLOSE_EVT  ) {} // Bluetooth client disconnected
  if (event == ESP_SPP_DATA_IND_EVT ) { // Data received over Bluetooth
    int received_duty = SerialBT.read();

    if ( received_duty <= 255 && received_duty >= 0 ) {
      duty = received_duty;

      ledcWrite(MOTOR_PIN_PWM, duty);
    }
  }
}

void setup(void) {
  Serial.begin(115200);
  SerialBT.begin(device_name); // Start Bluetooth communication with the device name
  SerialBT.register_callback(bluetoothCallback); // Register the callback function for Bluetooth events

  Serial.println("=============== Setup Start ===============");

  timer = timerBegin(1000000); // Timer 0, automatic divider calculation, Timer ticks every microsecond(us): 1/F. 80Mhz - 1Hz
  timerAttachInterrupt(timer, &timerInterrupt); // Attach the interrupt handling function

  pinMode(MOTOR_PIN_1, OUTPUT);
  pinMode(MOTOR_PIN_2, OUTPUT);
  ledcAttach(MOTOR_PIN_PWM, PWM_FREQUENCY, PWM_RESOLUTION);

  digitalWrite(MOTOR_PIN_1, 0);
  digitalWrite(MOTOR_PIN_2, 1);

  pinMode(ENCODER_PIN, INPUT_PULLUP);
  attachInterrupt(ENCODER_PIN, encoderInterrupt, RISING);

  ledcWrite(MOTOR_PIN_PWM, 255);

  delay(1000);

  Serial.println("=============== Setup End ===============");
}

void loop() {
  ledcWrite(MOTOR_PIN_PWM, 110);

  delay(500);

  start_time = millis();
  start = true;

  timerAlarm(timer, 10000, true, 0); // Set up alarm for 10ms based on the frecuency of 1Mhz, 50000us, repeating

  delay(250);

  ledcWrite(MOTOR_PIN_PWM, 250);

  delay(250);

  timerStop(timer);

  start = false;

  ledcWrite(MOTOR_PIN_PWM, 0);

  delay(1000000);
}
