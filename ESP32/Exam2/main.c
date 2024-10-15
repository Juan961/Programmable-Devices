// https://github.com/espressif/arduino-esp32/blob/master/libraries/BluetoothSerial/src/BluetoothSerial.cpp

#include <Adafruit_MPU6050.h> // MPU6050 sensor (accelerometer and gyroscope)
#include <Adafruit_Sensor.h> // Adafruit sensor support
#include <Wire.h> // I2C communication
#include <ESP32Servo.h> // Control servo with the ESP32
#include <BluetoothSerial.h> // Bluetooth Serial communication on ESP32

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

Adafruit_MPU6050 mpu;

Servo servo;

const int pinServo = 18;

const float accxCorrection = -0.01;
const float accyCorrection = -0.5;
const float acczCorrection = -0.29;

bool follow = true;

float angles[10] = {};

float getAngle() {
  sensors_event_t a, g, temp;

  mpu.getEvent(&a, &g, &temp);

  float accelerationx = a.acceleration.x + accxCorrection;
  float accelerationy = a.acceleration.y + accyCorrection;
  float accelerationz = a.acceleration.z + acczCorrection;

  float angle = degrees( atan(accelerationy / (sqrt(accelerationx * accelerationx + accelerationz * accelerationz))) );

  return angle;
}

float updateAngles(float lastAngle) {
  // Complete the vector of the last 10 angles to filter
  for ( int i = 1; i < 10; i++ ) {
    angles[i - 1] = angles[i];
  }

  angles[9] = lastAngle;

  float newAngle = angles[0];

  for ( int i = 1; i < 10; i++ ) {
    newAngle = (newAngle + angles[i]) / 2;
  }

  return newAngle;
}

void callback(esp_spp_cb_event_t event, esp_spp_cb_param_t *param ) {
  if (event == ESP_SPP_START_EVT) { // Bluetooth Serial Port Profile initialized
    Serial.println("Initialized SPP");
  } else if (event == ESP_SPP_SRV_OPEN_EVT ) { // Bluetooth client connected
    Serial.println("Client connected"); 
  } else if (event == ESP_SPP_CLOSE_EVT  ) { // Bluetooth client disconnected
    Serial.println("Client disconnected"); 
  } else if (event == ESP_SPP_DATA_IND_EVT ) {
    Serial.println("Data received"); // Data received over Bluetooth

    // Read a single byte from the incoming Bluetooth data buffer
    // This reads the next byte received via Bluetooth (range 0-255)
    int message = SerialBT.read();

    if (message == 0) {
      follow = !follow;
    }
  }
}

void setup(void) {
  // This initializes serial communication between the ESP32 and the computer (or serial monitor) 
  // at a baud rate of 115200 bits per second (bps). The baud rate determines how fast data is 
  // transmitted between the two devices. A higher baud rate like 115200 allows for faster 
  // communication and quicker data transfer, which is useful for applications that need to 
  // send or receive a lot of information in real time. It's important to match this baud rate 
  // with the serial monitor or any other connected device for correct communication.
  Serial.begin(115200);
  SerialBT.begin(device_name); // Start Bluetooth communication with the device name
  SerialBT.register_callback(callback); // Register the callback function for Bluetooth events

  Serial.println("=============== Setup Start ===============");

  // Initialize the MPU6050 sensor
  if (!mpu.begin(0x68)) { // Check if the sensor is found at address 0x68
    Serial.println("Failed to find MPU6050 chip"); // Print error if not found
    while (1) {
      delay(10); // Pause execution in case of failure
    }
  }

  // Set accelerometer range to ±8g
  // This configures the accelerometer in the MPU6050 sensor to measure acceleration 
  // within a range of ±8g (g is the acceleration due to gravity). The sensor will detect 
  // forces up to 8 times the force of gravity. Higher ranges like this allow the sensor 
  // to capture more intense movements (e.g., sudden accelerations), but at the cost of 
  // some sensitivity to smaller movements. Other ranges (±2g, ±4g, ±16g) can be used 
  // based on the precision or intensity of the movements you want to measure.
  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);

  // Set gyroscope range to ±500 degrees per second
  // This configures the gyroscope to measure angular velocity (rotation speed) up to 
  // ±500 degrees per second. This means the sensor can detect how fast the device is 
  // rotating along the x, y, and z axes, up to 500 degrees per second in either direction.
  // This range provides a good balance between capturing both slow and moderate-speed 
  // rotations. Other options like ±250 dps, ±1000 dps, and ±2000 dps offer trade-offs 
  // between sensitivity and the ability to detect faster rotations.
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);

  // Set low-pass filter bandwidth to 5Hz
  // This sets a low-pass filter for the accelerometer and gyroscope, allowing only signals 
  // with a frequency lower than 5Hz to pass through, while filtering out higher frequency 
  // signals (such as noise or rapid vibrations). A lower bandwidth like 5Hz is ideal for 
  // applications that prioritize smooth, stable data over rapid response to fast movements. 
  // Higher filter bandwidths (e.g., 10Hz, 21Hz, 44Hz) would allow for more responsive 
  // but potentially noisier data. In this case, the 5Hz filter ensures that only slow and 
  // deliberate movements are captured clearly, minimizing high-frequency noise.
  mpu.setFilterBandwidth(MPU6050_BAND_5_HZ);

  servo.attach(pinServo);

  // Complete the vector of the last 10 angles to filter
  for ( int i = 0; i < 10; i++ ) {
    angles[i] = getAngle();
  }

  Serial.println("=============== Setup End ===============");
}

void loop() {
  float lastAngle = getAngle();

  float angle = updateAngles(lastAngle);

  int servoAngle = 0;
  if ( angle > 90 || angle < -90) {

  } else if ( angle < 0 ) {
    servoAngle = 90 - abs(angle);
  } else {
    servoAngle = 180 - ( 90 - abs(angle));
  }

  if ( follow ) {
    servo.write(servoAngle);
    delay(50);
  }

  SerialBT.println(angle); // Send data to bluetooth

  delay(75);
}

// Baud: It is the transmission speed of symbols (signal changes) per second. It refers to the number of changes in the state of the signal per second.

// UART
// 8N1 is a common abbreviation for the configuration of the serial port in asynchronous mode.
// Start bit: Marks the beginning of a transmission, always has a value of 0.
// Data bits: These are the bits that contain the actual information in a serial transmission.
// Parity: This is an additional bit that checks whether the data was transmitted correctly (even or odd parity).
// Stop bit: Marks the end of a data frame.

// I2C
// I2C uses 7 or 8-bit addresses to identify each device on the bus, allowing multiple devices to connect to the same line.
// Start bit: Marks the beginning of a transmission, typically represented by a start condition signaling the bus to prepare for data transmission.
// Data bits: The bits that contain the actual information in a transmission; can be 8 bits, though the data size may vary depending on implementation.
// Acknowledgment (ACK/NACK) bit: After sending data, the receiver sends an acknowledgment (ACK) bit to indicate that the data was received correctly. If not received, a no acknowledgment (NACK) bit is sent.
// Stop bit: Marks the end of a transmission and is indicated by a stop condition, signaling devices that the transmission has concluded.

// SPI
// SPI uses multiple lines for communication: MISO, MOSI, SCLK, and CS for data transmission between the master and slaves.
// Start bit: SPI does not use a start bit since synchronization is achieved through the clock signal (SCLK).
// Data bits: The bits that contain the actual information in a transmission; typically sent in 8-bit blocks, though size may vary.
// No parity bits: SPI does not utilize parity bits; data transmission is controlled by the clock signal.
// Stop bit: SPI does not have a stop bit; transmission is determined by the duration of clock pulses and is considered complete when data ceases to be sent.
