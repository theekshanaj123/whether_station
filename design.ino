#include "DHT.h"
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Wire.h>

#define DHTPIN 2 // DHT sensor
#define DHTTYPE DHT11

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

#define TRIG_PIN 9
#define ECHO_PIN 10

#define BUZZER_PIN 12

#define LDR_PIN A0
#define LED_PIN 3

#define PUSH_BUTTON 5

DHT dht(DHTPIN, DHTTYPE);

long duration;
float distance;
boolean setButton = false;

void readDistance();

void setup()
{
  Serial.begin(9600);
  dht.begin();

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  pinMode(LDR_PIN, INPUT);

  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);

  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(PUSH_BUTTON, INPUT_PULLUP);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
  {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ;
  }

  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(30, 20);
  display.println(F("Whether   station!"));
  display.display();
  display.clearDisplay();

  delay(2000);
}

void loop()
{
  float light = analogRead(LDR_PIN);
  int mapVal = map(light, 0, 200, 1, 100);

  int buttonState = digitalRead(PUSH_BUTTON);

  if (buttonState == 0)
  {
    Serial.print("Button  :");
    Serial.println(buttonState);
    setButton = !setButton;
    delay(50); 
  }

  Serial.print("Set Button  :");
  Serial.println(setButton);

  if (mapVal < 10)
  {
    digitalWrite(LED_BUILTIN, HIGH);
  }
  else
  {
    digitalWrite(LED_BUILTIN, LOW);
  }

  float h = dht.readHumidity();
  float t = dht.readTemperature();

  // Check if any reads failed
  if (isnan(h) || isnan(t))
  {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }

  // Serial.print(F("Humidity: "));
  // Serial.print(h);
  // Serial.print(F("%  Temperature: "));
  // Serial.print(t);
  // Serial.println("");

  readDistance();

  if (distance <= 10)
  {
    tone(BUZZER_PIN, 500); // 1kHz beep
    delay(100);
    noTone(BUZZER_PIN);
    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(1, 25);
    display.println(F("Emergency! "));
    display.display();
    display.clearDisplay();
  }
  else if (setButton)
  {
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(10, 5);
    display.println(F("Water Level"));
    display.setCursor(10, 25);
    display.setTextSize(2);
    display.print(distance);
    display.println(F(" cm"));
    display.display();
  }
  else
  {
    noTone(BUZZER_PIN); // stop beep when object far away
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

  delay(70);
}

void readDistance()
{
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
