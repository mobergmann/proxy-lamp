enum FadingDirection {
  ASCENDING,
  DESCENDING
};

const size_t transistor_pin = 22;

float brightness = 0;
FadingDirection direction = FadingDirection::ASCENDING;

size_t scale(float in) {
  return in * in * in * 255;
}

void setup() {
  Serial.begin(9600);
  Wire.begin();

  pinMode(transistor_pin, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
}


void loop()
{
  analogWrite(transistor_pin, scale(brightness));

  delay(20);

  if (direction == FadingDirection::ASCENDING) {
    brightness += 0.01;
    if (brightness >= 1) {
      direction = FadingDirection::DESCENDING;
    }
  } else {
    brightness -= 0.01;
    if (brightness <= 0) {
      direction = FadingDirection::ASCENDING;
    }
  }
}