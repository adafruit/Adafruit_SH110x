/*!
 * @file SH1115_128x64_i2c_example.ino
 *
 * Example sketch for SH1115 128x64 OLED display using I2C interface.
 *
 * This demonstrates basic usage of the SH1115 display including:
 * - Basic text display
 * - Graphics primitives
 * - SH1115-specific features (breathing effect, power save)
 *
 * Hardware connections (I2C):
 * - VCC -> 3.3V or 5V (check your display)
 * - GND -> GND
 * - SCL -> SCL (or A5 on Arduino Uno)
 * - SDA -> SDA (or A4 on Arduino Uno)
 * - Optional: RST -> Digital pin (or connect to VCC)
 *
 * Tested with SH1115 (CH1115) 128x64 OLED displays
 */

#include <Adafruit_GFX.h>
#include <Adafruit_SH1115.h>
#include <Wire.h>

// Define display dimensions
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

// Define reset pin (set to -1 if not used or tied to VCC)
#define OLED_RESET -1

// Create display object
// Default I2C address is 0x3C (can be 0x3D if SA0 pin is high)
Adafruit_SH1115 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void setup() {
  Serial.begin(9600);

  // Wait for serial port to connect (needed for native USB)
  while (!Serial && millis() < 3000) {
    delay(10);
  }

  Serial.println("SH1115 128x64 OLED Test");

  // Initialize display with I2C address 0x3C
  // If your display uses 0x3D, change to: display.begin(0x3D)
  if (!display.begin(0x3C, true)) {
    Serial.println(F("SH1115 allocation failed"));
    for (;;)
      ; // Don't proceed, loop forever
  }

  Serial.println("Display initialized!");

  // Show initial display buffer contents on the screen
  // The library initializes with an Adafruit splash screen.
  display.display();
  delay(2000); // Pause for 2 seconds

  // Clear the buffer
  display.clearDisplay();

  // Run through examples
  testText();
  delay(2000);

  testGraphics();
  delay(2000);

  testSH1115Features();
  delay(2000);

  testAnimation();
}

void loop() {
  // Nothing in loop for this example
}

void testText() {
  display.clearDisplay();

  display.setTextSize(1);
  display.setTextColor(SH110X_WHITE);
  display.setCursor(0, 0);
  display.println(F("SH1115 OLED"));
  display.println(F("128x64 pixels"));
  display.println();
  display.println(F("Text size 1"));

  display.setTextSize(2);
  display.println(F("Size 2!"));

  display.display();
  Serial.println("Text test complete");
}

void testGraphics() {
  display.clearDisplay();

  // Draw rectangles
  display.drawRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, SH110X_WHITE);
  display.fillRect(10, 10, 20, 20, SH110X_WHITE);

  // Draw circles
  display.drawCircle(60, 20, 10, SH110X_WHITE);
  display.fillCircle(90, 20, 8, SH110X_WHITE);

  // Draw lines
  display.drawLine(0, 35, SCREEN_WIDTH - 1, 35, SH110X_WHITE);
  display.drawLine(SCREEN_WIDTH / 2, 40, SCREEN_WIDTH / 2, SCREEN_HEIGHT - 1,
                   SH110X_WHITE);

  // Draw triangles
  display.drawTriangle(10, 50, 20, 63, 0, 63, SH110X_WHITE);
  display.fillTriangle(110, 50, 120, 63, 100, 63, SH110X_WHITE);

  display.display();
  Serial.println("Graphics test complete");
}

void testSH1115Features() {
  display.clearDisplay();

  display.setTextSize(1);
  display.setTextColor(SH110X_WHITE);
  display.setCursor(0, 0);
  display.println(F("SH1115 Features:"));
  display.println();
  display.println(F("- Breathing LED"));
  display.println(F("- Power saving"));
  display.println(F("- Scrolling"));
  display.println(F("- 256-step contrast"));

  display.display();

  // Demonstrate breathing effect
  Serial.println("Enabling breathing effect for 5 seconds");
  display.setBreathing(true, 0, 1); // Enable, max brightness, 2 frame interval
  delay(5000);
  display.setBreathing(false); // Disable breathing

  Serial.println("SH1115 features test complete");
}

void testAnimation() {
  display.clearDisplay();

  // Bouncing ball animation
  int ballX = SCREEN_WIDTH / 2;
  int ballY = SCREEN_HEIGHT / 2;
  int ballVX = 2;
  int ballVY = 2;
  int ballRadius = 5;

  Serial.println("Bouncing ball animation - 5 seconds");

  unsigned long startTime = millis();
  while (millis() - startTime < 5000) {
    display.clearDisplay();

    // Draw border
    display.drawRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, SH110X_WHITE);

    // Draw ball
    display.fillCircle(ballX, ballY, ballRadius, SH110X_WHITE);

    // Update position
    ballX += ballVX;
    ballY += ballVY;

    // Bounce off walls
    if (ballX - ballRadius <= 1 || ballX + ballRadius >= SCREEN_WIDTH - 1) {
      ballVX = -ballVX;
    }
    if (ballY - ballRadius <= 1 || ballY + ballRadius >= SCREEN_HEIGHT - 1) {
      ballVY = -ballVY;
    }

    display.display();
    delay(20);
  }

  Serial.println("Animation complete");
}