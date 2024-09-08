#include <Wire.h>
#include <VL53L0X.h>

struct Intervall {
  size_t min;
  size_t max;
};

/// pin to the transistor, which then switches the lamp
const uint8_t lamp_pin = PD3;

/// Distance sensor
VL53L0X sensor;

/// conservative measurement bounds the sensor is capable of
Intervall sensor_bounds{ 30, 1500 };

uint16_t old_distance = 0;

/// brightness should be in the intervall ]0,1[
/// scale the linear input brightness cubic, to give it a more natural scaling
void set_brightness(double brightness) {
  size_t val = pow(brightness, 3) * 255;
  analogWrite(lamp_pin, val);
}

// function, which triggers an endless error protocol.
// the led will blink and if a serial connection is established, prints an optional error message
void error_protocol(size_t frequency, String message = "", double brightness = .5) {
  if (Serial and message.length() > 0) {
    // Serial.printf("Error: %s\n", message);
    Serial.print("Error: ");
    Serial.println(message);
  }

  while (true) {
    set_brightness(brightness);
    delay(frequency);
    set_brightness(0);
    delay(frequency);
  }
}

void log(String message) {
  if (Serial) {
    Serial.print("Log: ");
    Serial.println(message);
  }
}

/// normalize the val given the bounds
double normalize(size_t val, Intervall bounds) {
  // todo: bounds check
  if (val < bounds.min or val > bounds.max) {
    error_protocol(3000, "In the normalize function is val out of bounds");
  }

  // prevent division by zero
  if (val == bounds.min) {
    return 0;
  }

  return static_cast<double>(val - bounds.min) / static_cast<double>(bounds.max - bounds.min);
}

// todo: get the median od n measurements, to increase the accuracy and reduce errors
uint16_t get_distance() {
  uint16_t val = sensor.readRangeContinuousMillimeters();

  if (sensor.timeoutOccurred()) {
    error_protocol(2000, "VL53L0X: TIMEOUT");  // todo: blocking here might be a dumb idea, as this can happen often
  }

  return val;
}

void setup() {
  Serial.begin(9600);
  Wire.begin();

  // indicate that the program is running
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);

  // setup the lamp/ transistor with a brightness of 0
  pinMode(lamp_pin, OUTPUT);
  set_brightness(1);

  // setup the distance sensor
  {
    sensor.setTimeout(500);

    if (not sensor.init()) {
      error_protocol(1000, "VL53L0X: Failed to detect and initialize sensor.");
    }

    sensor.startContinuous();
  }

  delay(1000);

  // todo: get many measureemtns and use median of them (5s setup time)
  //       as long as we take the measurements, blink light
  uint16_t init_distance = get_distance();
  if (init_distance >= sensor_bounds.max) {
    ; // do not update, as sensor_bounds.max is initialized with the maximum possible measurement
  }
  else {
    sensor_bounds.max = init_distance;
  }
  log(sprintf("The initial distance is %ld mm", String(init_distance).c_str()));
}

void loop() {
  uint16_t new_distance = get_distance();
  log(sprintf("Measured Distance %ld mm", String(new_distance).c_str()));

  if (new_distance != old_distance) {
    if (new_distance <= sensor_bounds.min) {
      set_brightness(0);
    } else if (new_distance >= sensor_bounds.max) {
      // explicitly do nothing, as we want to retain the current brightness and not reset it
    } else if (new_distance < sensor_bounds.max) {  // todo here just an else is sufficent, condition is redundant
      double n = normalize(new_distance, sensor_bounds);
      set_brightness(n);
      log(sprintf("Updating Brightness to %lf %%", String(n).c_str()));
    }

    old_distance = new_distance;
  }
}