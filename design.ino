#include "DHT.h"
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Wire.h>

#define DHTPIN 2  //DHT sensor
#define DHTTYPE DHT11

#define SCREEN_WIDTH 128  // OLED display width, in pixels
#define SCREEN_HEIGHT 64  // OLED display height, in pixels

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

#define TRIG_PIN 9
#define ECHO_PIN 10

#define BUTTON_PIN 3

#define BUZZER_PIN 12

#define LDR_PIN A0

DHT dht(DHTPIN, DHTTYPE);


long duration;
float distance;

void setup() {
  Serial.begin(9600);
  dht.begin();

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  pinMode(LDR_PIN, INPUT);

  pinMode(BUZZER_PIN, OUTPUT);

  pinMode(BUTTON_PIN, INPUT_PULLUP);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {  // Address 0x3C for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ;  // Don't proceed, loop forever
  }

  display.clearDisplay();
  display.setTextSize(2);               // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE);  // Draw white text
  display.setCursor(30, 20);              // Start at top-left corner
  display.println(F("Whether   station!"));
  display.display();
  display.clearDisplay();

  delay(2000);
}

void loop() {
  int buttonState = digitalRead(BUTTON_PIN);

  float light = analogRead(LDR_PIN);
  int mapVal = map(light, 0, 200, 1, 100);

  Serial.print("light  :");
  Serial.println(light);

  float h = dht.readHumidity();
  float t = dht.readTemperature();

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }

  // Serial.print(F("Humidity: "));
  // Serial.print(h);
  // Serial.print(F("%  Temperature: "));
  // Serial.print(t);
  // Serial.println("");

  readDistance();

  if (distance <= 10) {
    tone(BUZZER_PIN, 500);  // 1kHz beep
    delay(100);
    noTone(BUZZER_PIN);
    display.clearDisplay();
    display.setTextSize(2);               // Normal 1:1 pixel scale
    display.setTextColor(SSD1306_WHITE);  // Draw white text
    display.setCursor(1, 25);             // Start at top-left corner
    display.println(F("Emergency! "));
    display.display();
    display.clearDisplay();
  } else {
    noTone(BUZZER_PIN);  // stop beep when object far away
    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    display.print("H:");
    display.print(h);
    display.println("%");

    display.setCursor(0, 23);
    display.print("T:");
    display.print(t);
    display.println("C");

    display.setCursor(0, 45);
    display.print("Light:");
    display.print(mapVal);
    display.println("%");

    display.display();
  }

  delay(100);
}

void readDistance() {
  // Send a 10µs pulse to trigger pin
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  // Read pulse on Echo pin
  duration = pulseIn(ECHO_PIN, HIGH);

  // Calculate distance in cm
  distance = duration * 0.034 / 2;

  // Serial.print("Distance: ");
  // Serial.print(distance);
  // Serial.println(" cm");
}