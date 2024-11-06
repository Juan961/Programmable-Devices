// https://espressif-docs.readthedocs-hosted.com/projects/arduino-esp32/en/latest/api/timer.html
// https://docs.espressif.com/projects/arduino-esp32/en/latest/migration_guides/2.x_to_3.0.html

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

hw_timer_t *timer = NULL;

int duty = 0;

void ARDUINO_ISR_ATTR timerInterrupt() {
  Serial.print("Duty stored: ");
  Serial.println(duty);
}

void bluetoothCallback(esp_spp_cb_event_t event, esp_spp_cb_param_t *param ) {
  // if (event == ESP_SPP_START_EVT) {} // Bluetooth Serial Port Profile initialized
  // else if (event == ESP_SPP_SRV_OPEN_EVT ) {} // Bluetooth client connected
  // else if (event == ESP_SPP_CLOSE_EVT  ) {} // Bluetooth client disconnected 
  if (event == ESP_SPP_DATA_IND_EVT ) { // Data received over Bluetooth
    duty = SerialBT.read();

    Serial.print("Duty received: ");
    Serial.println(duty);
  }
}

void setup(void) {
  Serial.begin(115200);
  SerialBT.begin(device_name); // Start Bluetooth communication with the device name
  SerialBT.register_callback(bluetoothCallback); // Register the callback function for Bluetooth events

  Serial.println("=============== Setup Start ===============");

  timer = timerBegin(1000000); // Timer 0, automatic divider calculation, Timer ticks every microsecond(us): 1/F. 80Mhz - 1Hz
  timerAttachInterrupt(timer, &timerInterrupt); // Attach the interrupt handling function
  timerAlarm(timer, 50000, true, 0); // Set up alarm for 50ms based on the frecuency of 1Mhz, 50000us, repeating

  Serial.println("=============== Setup End ===============");
}

void loop() {
  delay(75);
}
