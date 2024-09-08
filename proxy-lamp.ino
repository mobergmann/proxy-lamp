#include <Wire.h>
#include <VL53L0X.h>

struct Intervall {
  size_t min;
  size_t max;
};

/// pin to the transistor, which then switches the lamp
const uint8_t lamp_pin = 22;

/// Distance sensor
// VL53L0X sensor;

Intervall sensor_bounds{ 0, 50 };

/// brightness should be in the intervall ]0,1[
/// scale the linear input brightness cubic, to give it a more natural scaling
void set_brightness(double brightness) {
  size_t val = pow(brightness, 3) * 255;
  analogWrite(lamp_pin, val);
}

/// normalize the val given the bounds
double normalize(size_t val, Intervall bounds) {
  // todo: bounds check
  
  if (val == bounds.min) {
    return 0;
  }

  return (double)(val - bounds.min) / (double)(bounds.max - bounds.min);
}

void setup() {
  Serial.begin(9600);
  while (not Serial) {
    ;
  }

  Serial.println("1");

  Wire.begin();

  Serial.println("2");

  // indicate that the program is running
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);

  // setup the transistor
  
  pinMode(lamp_pin, OUTPUT);

  Serial.println("3");

  // setup the distance sensor
  // {
  //   sensor.setTimeout(500);
  //   if (not sensor.init()) {
  //     Serial.println("Failed to detect and initialize sensor!");
  //     while (true) {
  //       // todo: flash in 1s interval to indicate error
  //       // todo: turn that into an error function
  //     }
  //   }

  //   sensor.startContinuous();
  // }

  Serial.println("4");
}

void loop() {
  uint16_t distance = 10;//sensor.readRangeContinuousMillimeters();

  // if (sensor.timeoutOccurred()) {
  //   // todo: flash in 1s interval to indicate error
  //   // todo: turn that into an error function

  //   Serial.print("");
  //   Serial.print(" TIMEOUT");
  //   Serial.println("");
  // }

  if (distance < sensor_bounds.max) {
    double n = normalize(distance, sensor_bounds);
    Serial.printf("n: %f, distance: %d, max: %d, min: %d\n", n, distance, sensor_bounds.max, sensor_bounds.min);
    set_brightness(n);
  }
}