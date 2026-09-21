#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// HC-SR04
#define TRIG_PIN 5
#define ECHO_PIN 18

// LEDs and buzzer
#define GREEN_LED 2
#define RED_LED 4
#define BUZZER 15

// Tank height in cm
#define TANK_HEIGHT 30.0

void setup()
{
  Serial.begin(115200);

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  pinMode(GREEN_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);
  pinMode(BUZZER, OUTPUT);

  // Start OLED
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
  {
    Serial.println("OLED not found!");
    while (1);
  }

  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(2);

  display.setCursor(10, 10);
  display.println("WATER");

  display.setCursor(10, 35);
  display.println("TANK");

  display.display();

  delay(2000);
}

void loop()
{
  // -------------------------
  // Ultrasonic measurement
  // -------------------------

  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);

  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);

  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH, 30000);

  // If no echo is received
  if (duration == 0)
  {
    Serial.println("No ultrasonic reading");
    return;
  }

  float distance = duration * 0.0343 / 2;

  // Calculate water height
  float waterHeight = TANK_HEIGHT - distance;

  if (waterHeight < 0)
    waterHeight = 0;

  if (waterHeight > TANK_HEIGHT)
    waterHeight = TANK_HEIGHT;

  // Calculate percentage
  float percentage = (waterHeight / TANK_HEIGHT) * 100;

  Serial.print("Water Level: ");
  Serial.print(percentage);
  Serial.println("%");

  // -------------------------
  // LED + BUZZER CONTROL
  // -------------------------

  if (percentage < 50)
  {
    // Below 50%
    digitalWrite(GREEN_LED, LOW);
    digitalWrite(RED_LED, LOW);
    digitalWrite(BUZZER, LOW);
  }
  else if (percentage < 80)
  {
    // 50% to 79%
    digitalWrite(RED_LED, LOW);
    digitalWrite(BUZZER, LOW);

    digitalWrite(GREEN_LED, HIGH);
    delay(300);

    digitalWrite(GREEN_LED, LOW);
    delay(300);
  }
  else
  {
    // 80% and above
    digitalWrite(GREEN_LED, LOW);
    digitalWrite(RED_LED, HIGH);
    digitalWrite(BUZZER, HIGH);
  }

  // -------------------------
  // OLED DISPLAY
  // -------------------------

  display.clearDisplay();

  display.setTextColor(SSD1306_WHITE);

  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("WATER LEVEL");

  display.setTextSize(3);
  display.setCursor(25, 15);
  display.print((int)percentage);
  display.println("%");

  display.setTextSize(1);
  display.setCursor(0, 50);

  if (percentage < 50)
  {
    display.println("STATUS: NORMAL");
  }
  else if (percentage < 80)
  {
    display.println("STATUS: MEDIUM");
  }
  else
  {
    display.println("STATUS: HIGH!");
  }

  display.display();

  delay(200);
}
