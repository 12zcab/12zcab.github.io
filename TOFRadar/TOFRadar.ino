#include <Wire.h>
#include <VL53L0X.h>
#include <ArduinoJson.h>

// Flexible structure array implementation targeting modular additions
struct ToFSensor {
  VL53L0X sensor;
  int xshutPin;      // GPIO pin tied to physical sensor XSHUT
  int address;       // Designated runtime unique I2C address map
  int degree;        // Structural vector degree mapping layout property
  bool isReady;      // Internal operational check flag
};

// ESP32-C3 Supermini Mapping Reference
// Simply add/remove lines here to scale up your project array
ToFSensor sensors[] = {
  {VL53L0X(), 0, 0x30, 0, false},   // Sensor 1: Bound to GPIO 0 at 0 deg orientation
  {VL53L0X(), 1, 0x31, 90, false},  // Sensor 2: Bound to GPIO 1 at 90 deg orientation
  {VL53L0X(), 2, 0x32, 180, false}  // Sensor 3: Bound to GPIO 2 at 180 deg orientation
};

const int totalSensors = sizeof(sensors) / sizeof(sensors);

// Non-blocking timer tracking variable
unsigned long lastTransmissionTime = 0;
const unsigned long transmissionInterval = 100; // Send telemetry package every 100ms

void initSensors() {
  // Force reset status states out to physical pins
  for (int i = 0; i < totalSensors; i++) {
    pinMode(sensors[i].xshutPin, OUTPUT);
    digitalWrite(sensors[i].xshutPin, LOW);
  }
  delay(15);

  // Address reassignment iterations sequentially
  for (int i = 0; i < totalSensors; i++) {
    // Release isolated hardware line targeted for current setup iteration
    pinMode(sensors[i].xshutPin, INPUT);
    delay(15);

    sensors[i].sensor.setTimeout(200);
    if (!sensors[i].sensor.init()) {
      // Diagnostic fallback tracking if a sensor fails during startup
      continue; 
    }

    // Change address profile target mapping away from standard baseline configurations (0x29)
    sensors[i].sensor.setAddress(sensors[i].address);
    sensors[i].isReady = true;

    // Trigger high-speed background tracking telemetry mode
    sensors[i].sensor.startContinuous();
  }
}

void setup() {
  // ESP32-C3 uses native USB Serial. 115200 is used as a baseline standard.
  Serial.begin(115200);

  // ESP32-C3 Supermini Default Pin initialization mapping structure (SDA=GPIO 8, SCL=GPIO 9)
  Wire.begin(8, 9);

  // Sequence operational array blocks
  initSensors();
}

void loop() {
  unsigned long currentTime = millis();

  // Non-blocking loop pacing telemetry dispatch intervals
  if (currentTime - lastTransmissionTime >= transmissionInterval) {
    lastTransmissionTime = currentTime;

    // Allocate JSON Document dynamically matching sensor count
    JsonDocument doc;
    JsonArray sensorArray = doc.to<JsonArray>();

    for (int i = 0; i < totalSensors; i++) {
      JsonObject sensorObject = sensorArray.add<JsonObject>();
      sensorObject["degree"] = sensors[i].degree;
      
      if (sensors[i].isReady) {
        uint16_t dist = sensors[i].sensor.readRangeContinuousMillimeters();
        if (sensors[i].sensor.timeoutOccurred()) {
          sensorObject["distance"] = -1; // -1 represents structural hardware Timeout
        } else {
          sensorObject["distance"] = dist;
        }
      } else {
        sensorObject["distance"] = -2; // -2 represents structural hardware Offline
      }
    }

    // Output clean minified JSON down the USB streaming pipe ending with a newline
    serializeJson(doc, Serial);
    Serial.println(); 
  }
}
