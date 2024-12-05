/**
 * Truck Load Monitoring System
 * 
 * This code reads load cell data using the HX711 amplifier and displays it on a touchscreen interface.
 * It keeps track of the total number of loads and total weight, storing data in EEPROM.
 * Includes buttons for Tare, Store, Reset, and Calibrate.
 * 
 * Hardware Components:
 * - Arduino Uno
 * - HX711 Load Cell Amplifier
 * - Load Cell Sensor
 * - Adafruit 2.8" TFT Touch Shield for Arduino
 * 
 * Libraries Required:
 * - HX711 Library by Bogdan Necula
 * - Adafruit GFX Library
 * - Adafruit TouchScreen Library
 * - MCUFRIEND_kbv Library
 * - EEPROM Library (built-in)
 */

#include "HX711.h"
#include <Adafruit_GFX.h>
#include <MCUFRIEND_kbv.h>
#include <TouchScreen.h>
#include <EEPROM.h>

// HX711 pins
#define HX711_DT  3
#define HX711_SCK 2

// Initialize HX711
HX711 scale(HX711_DT, HX711_SCK);

// Calibration factor for the scale (You need to calibrate this for your setup)
float calibration_factor = -7050; // Initial calibration factor

// Touchscreen pins, XP, XM, YP, YM
#define YP A3  // must be an analog pin, use "An" notation!
#define XM A2  // must be an analog pin, use "An" notation!
#define YM 9   // can be a digital pin
#define XP 8   // can be a digital pin

#define TS_MINX 150
#define TS_MINY 120
#define TS_MAXX 920
#define TS_MAXY 940

// Touchscreen setup
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);
MCUFRIEND_kbv tft;

// EEPROM Addresses
#define EEPROM_LOAD_COUNT_ADDR 0        // Addresses 0 and 1
#define EEPROM_TOTAL_WEIGHT_ADDR 2      // Addresses 2 to 5
#define EEPROM_CAL_FACTOR_ADDR 6        // Addresses 6 to 9

// Variables for load tracking
float last_weight = 0;
float current_weight = 0;
float total_weight = 0;
int load_count = 0;

// State variables
bool load_detected = false;

// Button coordinates and dimensions
#define BUTTON_W 100
#define BUTTON_H 40

// Tare button
#define TARE_BUTTON_X 10
#define TARE_BUTTON_Y 200

// Store Values button
#define STORE_BUTTON_X 120
#define STORE_BUTTON_Y 200

// Reset All button
#define RESET_BUTTON_X 230
#define RESET_BUTTON_Y 200

// Calibrate button
#define CALIBRATE_BUTTON_X 340
#define CALIBRATE_BUTTON_Y 200

// Calibration variables
bool isCalibrating = false;
String enteredWeight = "";

void setup() {
  Serial.begin(9600);

  // Read stored calibration factor from EEPROM
  calibration_factor = EEPROMReadFloat(EEPROM_CAL_FACTOR_ADDR);
  if (isnan(calibration_factor) || calibration_factor == 0) {
    calibration_factor = -7050; // Default value
  }
  scale.set_scale(calibration_factor);
  scale.tare();  // Reset the scale to 0

  // Read stored values from EEPROM
  load_count = EEPROMReadInt(EEPROM_LOAD_COUNT_ADDR);
  total_weight = EEPROMReadFloat(EEPROM_TOTAL_WEIGHT_ADDR);

  uint16_t ID = tft.readID();
  tft.begin(ID);

  tft.setRotation(1);
  tft.fillScreen(BLACK);

  // Draw initial UI
  drawUI();
}

void loop() {
  if (isCalibrating) {
    handleCalibration();
  } else {
    // Read the scale
    current_weight = scale.get_units();

    // Simple filter to remove noise around zero
    if (abs(current_weight) < 0.5) {
      current_weight = 0;
    }

    // Detect load weight when scale goes back to zero
    if (current_weight > 0) {
      load_detected = true;
      last_weight = current_weight;
    } else if (current_weight == 0 && load_detected) {
      // Load has been unloaded
      total_weight += last_weight;
      load_count++;
      last_weight = 0;
      load_detected = false;
    }

    // Update display
    updateDisplay();

    // Handle touch input
    TSPoint p = ts.getPoint();
    pinMode(XM, OUTPUT);
    pinMode(YP, OUTPUT);

    if (p.z > ts.pressureThreshhold) {
      // Map touchscreen coordinates
      int x = map(p.x, TS_MINX, TS_MAXX, 0, tft.width());
      int y = map(p.y, TS_MINY, TS_MAXY, 0, tft.height());

      // Check if Tare button was pressed
      if (x > TARE_BUTTON_X && x < (TARE_BUTTON_X + BUTTON_W) && y > TARE_BUTTON_Y && y < (TARE_BUTTON_Y + BUTTON_H)) {
        scale.tare();
        current_weight = 0;
        last_weight = 0;
        load_detected = false;
        tft.fillRect(0, 0, tft.width(), 160, BLACK);  // Clear the upper part of the screen
      }

      // Check if Store Values button was pressed
      else if (x > STORE_BUTTON_X && x < (STORE_BUTTON_X + BUTTON_W) && y > STORE_BUTTON_Y && y < (STORE_BUTTON_Y + BUTTON_H)) {
        // Store current total_weight and load_count to EEPROM
        EEPROMWriteInt(EEPROM_LOAD_COUNT_ADDR, load_count);
        EEPROMWriteFloat(EEPROM_TOTAL_WEIGHT_ADDR, total_weight);
        tft.fillRect(0, 160, tft.width(), 20, BLACK);  // Clear notification area
        tft.setCursor(20, 160);
        tft.setTextColor(GREEN);
        tft.setTextSize(2);
        tft.print("Values Stored");
      }

      // Check if Reset All button was pressed
      else if (x > RESET_BUTTON_X && x < (RESET_BUTTON_X + BUTTON_W) && y > RESET_BUTTON_Y && y < (RESET_BUTTON_Y + BUTTON_H)) {
        // Reset all values and clear EEPROM
        total_weight = 0;
        load_count = 0;
        last_weight = 0;
        current_weight = 0;
        load_detected = false;

        // Clear EEPROM values
        EEPROMWriteInt(EEPROM_LOAD_COUNT_ADDR, load_count);
        EEPROMWriteFloat(EEPROM_TOTAL_WEIGHT_ADDR, total_weight);

        // Clear display
        tft.fillRect(0, 0, tft.width(), 160, BLACK);  // Clear the upper part of the screen

        tft.fillRect(0, 160, tft.width(), 20, BLACK);  // Clear notification area
        tft.setCursor(20, 160);
        tft.setTextColor(GREEN);
        tft.setTextSize(2);
        tft.print("All Values Reset");
      }

      // Check if Calibrate button was pressed
      else if (x > CALIBRATE_BUTTON_X && x < (CALIBRATE_BUTTON_X + BUTTON_W) && y > CALIBRATE_BUTTON_Y && y < (CALIBRATE_BUTTON_Y + BUTTON_H)) {
        isCalibrating = true;
        startCalibration();
      }
    }

    delay(200);
  }
}

void drawUI() {
  // Draw Tare Button
  tft.fillRect(TARE_BUTTON_X, TARE_BUTTON_Y, BUTTON_W, BUTTON_H, BLUE);
  tft.drawRect(TARE_BUTTON_X, TARE_BUTTON_Y, BUTTON_W, BUTTON_H, WHITE);
  tft.setTextColor(WHITE);
  tft.setTextSize(2);
  tft.setCursor(TARE_BUTTON_X + 10, TARE_BUTTON_Y + 10);
  tft.print("Tare");

  // Draw Store Values Button
  tft.fillRect(STORE_BUTTON_X, STORE_BUTTON_Y, BUTTON_W, BUTTON_H, BLUE);
  tft.drawRect(STORE_BUTTON_X, STORE_BUTTON_Y, BUTTON_W, BUTTON_H, WHITE);
  tft.setTextColor(WHITE);
  tft.setTextSize(2);
  tft.setCursor(STORE_BUTTON_X + 5, STORE_BUTTON_Y + 10);
  tft.print("Store");

  // Draw Reset All Button
  tft.fillRect(RESET_BUTTON_X, RESET_BUTTON_Y, BUTTON_W, BUTTON_H, RED);
  tft.drawRect(RESET_BUTTON_X, RESET_BUTTON_Y, BUTTON_W, BUTTON_H, WHITE);
  tft.setTextColor(WHITE);
  tft.setTextSize(2);
  tft.setCursor(RESET_BUTTON_X + 5, RESET_BUTTON_Y + 10);
  tft.print("Reset");

  // Draw Calibrate Button
  tft.fillRect(CALIBRATE_BUTTON_X, CALIBRATE_BUTTON_Y, BUTTON_W, BUTTON_H, BLUE);
  tft.drawRect(CALIBRATE_BUTTON_X, CALIBRATE_BUTTON_Y, BUTTON_W, BUTTON_H, WHITE);
  tft.setTextColor(WHITE);
  tft.setTextSize(2);
  tft.setCursor(CALIBRATE_BUTTON_X + 5, CALIBRATE_BUTTON_Y + 10);
  tft.print("Calib");

  // Labels
  tft.setTextColor(WHITE);
  tft.setTextSize(2);
  tft.setCursor(20, 20);
  tft.print("Current Weight:");
  tft.setCursor(20, 60);
  tft.print("Total Loads:");
  tft.setCursor(20, 100);
  tft.print("Total Weight:");

  // Display initial values
  updateDisplay();
}

void updateDisplay() {
  // Current Weight
  tft.fillRect(200, 20, 120, 20, BLACK);
  tft.setCursor(200, 20);
  tft.setTextColor(GREEN);
  tft.setTextSize(2);
  tft.print(current_weight, 1);
  tft.print(" kg");

  // Total Loads
  tft.fillRect(200, 60, 120, 20, BLACK);
  tft.setCursor(200, 60);
  tft.setTextColor(GREEN);
  tft.setTextSize(2);
  tft.print(load_count);

  // Total Weight
  tft.fillRect(200, 100, 120, 20, BLACK);
  tft.setCursor(200, 100);
  tft.setTextColor(GREEN);
  tft.setTextSize(2);
  tft.print(total_weight / 1000, 1);  // Convert kg to tons
  tft.print(" tons");
}

void startCalibration() {
  // Clear screen and display instructions
  tft.fillScreen(BLACK);
  tft.setTextColor(WHITE);
  tft.setTextSize(2);
  tft.setCursor(20, 40);
  tft.print("Fill the truck");
  tft.setCursor(20, 70);
  tft.print("with known weight");
  tft.setCursor(20, 110);
  tft.print("Enter weight in kg:");

  // Draw a rectangle for the input field
  tft.drawRect(20, 140, 200, 30, WHITE);
  tft.setCursor(25, 145);
  tft.setTextColor(GREEN);
  tft.print(enteredWeight);

  // Draw keypad
  drawKeypad();
}

void handleCalibration() {
  TSPoint p = ts.getPoint();
  pinMode(XM, OUTPUT);
  pinMode(YP, OUTPUT);

  if (p.z > ts.pressureThreshhold) {
    // Map touchscreen coordinates
    int x = map(p.x, TS_MINX, TS_MAXX, 0, tft.width());
    int y = map(p.y, TS_MINY, TS_MAXY, 0, tft.height());

    // Handle keypad input
    char key = getKeypadInput(x, y);
    if (key != 0) {
      if (key >= '0' && key <= '9') {
        enteredWeight += key;
      } else if (key == '.') {
        if (!enteredWeight.contains(".")) {
          enteredWeight += key;
        }
      } else if (key == 'C') {
        // Clear input
        enteredWeight = "";
      } else if (key == 'E') {
        // Enter key pressed
        if (enteredWeight.length() > 0) {
          float knownWeight = enteredWeight.toFloat();

          // Read raw average value from the scale
          float rawValue = scale.read_average(10);

          // Calculate new calibration factor
          calibration_factor = rawValue / knownWeight;

          // Update the scale with the new calibration factor
          scale.set_scale(calibration_factor);

          // Save calibration factor to EEPROM
          EEPROMWriteFloat(EEPROM_CAL_FACTOR_ADDR, calibration_factor);

          // Display confirmation
          tft.fillScreen(BLACK);
          tft.setTextColor(WHITE);
          tft.setTextSize(2);
          tft.setCursor(20, 80);
          tft.print("Known weight is");
          tft.setCursor(20, 110);
          tft.print(knownWeight, 1);
          tft.print(" kg");
          tft.setCursor(20, 140);
          tft.print("Weight calibrated");

          delay(3000); // Wait for 3 seconds

          // Redraw the UI
          tft.fillScreen(BLACK);
          drawUI();
          isCalibrating = false;
          enteredWeight = "";
        }
      }

      // Update the input field
      tft.fillRect(21, 141, 198, 28, BLACK);
      tft.setCursor(25, 145);
      tft.setTextColor(GREEN);
      tft.print(enteredWeight);
    }
    delay(200);
  }
}

void drawKeypad() {
  // Define keypad buttons
  const char keys[4][3] = {
    {'1', '2', '3'},
    {'4', '5', '6'},
    {'7', '8', '9'},
    {'.', '0', 'E'} // E for Enter
  };

  int startX = 240;
  int startY = 40;
  int keyW = 60;
  int keyH = 40;
  int spacing = 10;

  for (int row = 0; row < 4; row++) {
    for (int col = 0; col < 3; col++) {
      int x = startX + col * (keyW + spacing);
      int y = startY + row * (keyH + spacing);

      // Draw key
      tft.fillRect(x, y, keyW, keyH, BLUE);
      tft.drawRect(x, y, keyW, keyH, WHITE);
      tft.setTextColor(WHITE);
      tft.setTextSize(2);
      tft.setCursor(x + 20, y + 10);
      tft.print(keys[row][col]);
    }
  }

  // Clear (C) button
  int x = startX;
  int y = startY + 4 * (keyH + spacing);
  tft.fillRect(x, y, keyW * 3 + spacing * 2, keyH, RED);
  tft.drawRect(x, y, keyW * 3 + spacing * 2, keyH, WHITE);
  tft.setTextColor(WHITE);
  tft.setTextSize(2);
  tft.setCursor(x + (keyW * 3 + spacing * 2) / 2 - 15, y + 10);
  tft.print("C");
}

char getKeypadInput(int x, int y) {
  // Define keypad buttons
  const char keys[4][3] = {
    {'1', '2', '3'},
    {'4', '5', '6'},
    {'7', '8', '9'},
    {'.', '0', 'E'} // E for Enter
  };

  int startX = 240;
  int startY = 40;
  int keyW = 60;
  int keyH = 40;
  int spacing = 10;

  for (int row = 0; row < 4; row++) {
    for (int col = 0; col < 3; col++) {
      int btnX = startX + col * (keyW + spacing);
      int btnY = startY + row * (keyH + spacing);
      if (x > btnX && x < (btnX + keyW) && y > btnY && y < (btnY + keyH)) {
        return keys[row][col];
      }
    }
  }

  // Check Clear (C) button
  int btnX = startX;
  int btnY = startY + 4 * (keyH + spacing);
  int btnW = keyW * 3 + spacing * 2;
  int btnH = keyH;
  if (x > btnX && x < (btnX + btnW) && y > btnY && y < (btnY + btnH)) {
    return 'C';
  }

  return 0; // No key pressed
}

// EEPROM read/write functions
void EEPROMWriteInt(int address, int value) {
  byte lowByte = value & 0xFF;
  byte highByte = (value >> 8) & 0xFF;
  EEPROM.update(address, lowByte);
  EEPROM.update(address + 1, highByte);
}

int EEPROMReadInt(int address) {
  byte lowByte = EEPROM.read(address);
  byte highByte = EEPROM.read(address + 1);
  return (highByte << 8) | lowByte;
}

void EEPROMWriteFloat(int address, float value) {
  byte *floatBytes = (byte *) &value;
  for (int i = 0; i < 4; i++) {
    EEPROM.update(address + i, floatBytes[i]);
  }
}

float EEPROMReadFloat(int address) {
  float value = 0.0;
  byte *floatBytes = (byte *) &value;
  for (int i = 0; i < 4; i++) {
    floatBytes[i] = EEPROM.read(address + i);
  }
  return value;
}
