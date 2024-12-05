# Truck Load Monitoring System ( Bubba load)

This project is an Arduino-based truck load monitoring system that reads data from a load cell using the HX711 amplifier. It allows you to:

- Display current load weight
- Count the total number of loads
- Calculate total weight in tons
- Store load data even after power off using EEPROM
- Calibrate the scale with a known weight
- Interact via a touchscreen interface with buttons for Tare, Store, Reset, and Calibrate

## Table of Contents

- [Introduction](#introduction)
- [Prerequisites](#prerequisites)
- [Hardware Components](#hardware-components)
- [Software Requirements](#software-requirements)
- [Hardware Setup](#hardware-setup)
- [Software Setup](#software-setup)
- [Uploading the Code](#uploading-the-code)
- [Using the System](#using-the-system)
- [Troubleshooting](#troubleshooting)
- [Optional: Using Visual Studio Code with PlatformIO](#optional-using-visual-studio-code-with-platformio)
- [License](#license)

## Introduction

This project provides a comprehensive solution for monitoring truck loads using an Arduino microcontroller. It's designed to be user-friendly, requiring minimal programming knowledge to set up and use.

## Prerequisites

- Basic understanding of electronics and wiring
- Familiarity with using the Arduino IDE or willingness to learn
- A Windows, macOS, or Linux computer with a USB port
- **Hardware Components** (links provided below for purchasing)

## Hardware Components

You will need the following hardware components:

1. **Arduino Uno** (or compatible board)

   - Purchase from the official Arduino website: [Arduino Uno](https://store.arduino.cc/products/arduino-uno-rev3)

2. **HX711 Load Cell Amplifier**

   - Purchase from SparkFun: [SparkFun Load Cell Amplifier - HX711](https://www.sparkfun.com/products/13879)
   - Or from Aliexpress: [HX711 Module on Aliexpress](https://www.aliexpress.com/item/1005007696183020.html)

3. **Load Cell Sensor**

   - Ensure it has an appropriate capacity for your truck's maximum load.
   - Available from various suppliers; for example:
     - [Load Cells on SparkFun](https://www.sparkfun.com/categories/469)
     - [Load Cells on Aliexpress](https://www.aliexpress.com/wholesale?SearchText=load+cell)

4. **Adafruit 2.8" TFT Touch Shield for Arduino** (Product ID: 1651)

   - Purchase from Adafruit: [Adafruit 2.8" TFT Touch Shield](https://www.adafruit.com/product/1651)

5. **Connecting Wires**

   - Jumper wires for making connections between components.
   - Available from various suppliers.

6. **Power Supply**

   - If not using USB power from the computer.
   - Ensure the power supply meets the voltage and current requirements of your setup.

**Note:** Prices and availability may vary based on your location. The provided links are for convenience and reference.

## Software Requirements

- **Arduino IDE** (version 1.8.0 or later)
  - Download from [Arduino Official Website](https://www.arduino.cc/en/software)

- **Arduino Libraries** (to be installed in the Arduino IDE)
  - **HX711 Library** by Bogdan Necula
  - **Adafruit GFX Library**
  - **Adafruit TouchScreen Library**
  - **MCUFRIEND_kbv Library**

**Optional:**

- **Visual Studio Code** with **PlatformIO** extension
  - Provides an alternative development environment
  - Download VSCode from [Visual Studio Code Website](https://code.visualstudio.com/)
  - Install PlatformIO from [PlatformIO Website](https://platformio.org/install/ide?install=vscode)

## Hardware Setup

### 1. Wiring the HX711 Load Cell Amplifier

**HX711 to Arduino:**

- **VCC** -> 5V
- **GND** -> GND
- **DT (Data)** -> Pin 3
- **SCK (Clock)** -> Pin 2

**Note:** Connect the load cell sensor to the HX711 according to the sensor's datasheet. Typically, the load cell wires are connected to the HX711 as follows:

- **Load Cell Red Wire (Excitation+ or E+)** -> HX711 E+
- **Load Cell Black Wire (Excitation- or E-)** -> HX711 E-
- **Load Cell White Wire (Signal+ or A+)** -> HX711 A+
- **Load Cell Green Wire (Signal- or A-)** -> HX711 A-

### 2. Connecting the Touchscreen Shield

- Simply stack the **Adafruit 2.8" TFT Touch Shield** onto the Arduino Uno. Ensure all pins are properly aligned.

## Software Setup

### 1. Install the Arduino IDE

- Download and install the Arduino IDE from the [Arduino Official Website](https://www.arduino.cc/en/software).

### 2. Install Required Libraries

#### a. HX711 Library

- Open the Arduino IDE.
- Go to **Sketch** -> **Include Library** -> **Manage Libraries...**
- In the Library Manager, search for **"HX711_ADC"** by **Bogdan Necula**.
- Click **Install**.

#### b. Adafruit GFX Library

- In the Library Manager, search for **"Adafruit GFX Library"**.
- Click **Install**.

#### c. MCUFRIEND_kbv Library

- In the Library Manager, search for **"MCUFRIEND_kbv"**.
- Click **Install**.

#### d. Adafruit TouchScreen Library

- In the Library Manager, search for **"Adafruit TouchScreen"**.
- Click **Install**.

### 3. Download the Code

- Download the code from this repository as a ZIP file.
- Extract the ZIP file to a folder on your computer.

## Uploading the Code

### 1. Open the Project in Arduino IDE

- Open the Arduino IDE.
- Go to **File** -> **Open...**
- Navigate to the folder where you extracted the code.
- Select the `.ino` file and open it.

### 2. Select Your Board and Port

- Go to **Tools** -> **Board** and select **"Arduino Uno"** (or your specific board if different).
- Go to **Tools** -> **Port** and select the COM port that your Arduino is connected to.

### 3. Upload the Code

- Click the **Upload** button (rightward arrow icon) in the Arduino IDE.
- Wait for the code to compile and upload to your Arduino board.
- If successful, you will see "Done uploading" in the status bar.

## Using the System

### 1. Power On

- Connect your Arduino to a power source (USB or external power supply).
- The touchscreen display should light up and show the interface.

### 2. Calibrate the Scale

- Press the **"Calib"** button on the touchscreen.
- Follow the on-screen instructions:
  - Fill the truck with a known weight.
  - Enter the known weight using the on-screen keypad.
- The system will calibrate the scale and save the calibration factor.

### 3. Tare the Scale (if necessary)

- Press the **"Tare"** button to reset the scale to zero.

### 4. Weighing Loads

- As you load and unload the truck, the system will display the current weight.
- When the scale returns to zero after unloading, it automatically records the load weight and increments the load count.

### 5. Storing Data

- Press the **"Store"** button to save the current total weight and load count to the EEPROM.
- Data saved to EEPROM will persist even after power loss.

### 6. Resetting Data

- Press the **"Reset"** button to clear all stored data and reset the system.

## Troubleshooting

- **No Display on Touchscreen:**
  - Ensure the touchscreen shield is properly connected to the Arduino.
  - Check that the correct libraries are installed.

- **Incorrect Weight Readings:**
  - Recalibrate the scale using the **"Calib"** button.
  - Verify that the load cell and HX711 are wired correctly.

- **Touchscreen Not Responding:**
  - Recheck the calibration constants in the code for the touchscreen.
  - Make sure the touch libraries are installed correctly.

- **Compilation Errors:**
  - Ensure all required libraries are installed.
  - Verify that you are using the correct board and port settings in the Arduino IDE.

## Optional: Using Visual Studio Code with PlatformIO

If you prefer, you can use Visual Studio Code with the PlatformIO extension as your development environment.

### Steps:

1. **Install Visual Studio Code**

   - Download from [Visual Studio Code Website](https://code.visualstudio.com/).

2. **Install PlatformIO Extension**

   - Open VSCode.
   - Go to **Extensions** (Ctrl+Shift+X).
   - Search for **"PlatformIO IDE"** and install it.

3. **Import Arduino Project**

   - Open PlatformIO Home (house icon in the bottom toolbar).
   - Click on **Open Project**.
   - Navigate to the folder containing your Arduino project and open it.
   - PlatformIO will configure the project for you.

4. **Install Libraries in PlatformIO**

   - Open the `platformio.ini` file in your project.
   - Add the required libraries under the `[env:your_board]` section, for example:

     ```ini
     [env:uno]
     platform = atmelavr
     board = uno
     framework = arduino
     lib_deps =
       bogde/HX711@^0.7.5
       adafruit/Adafruit GFX Library@^1.10.10
       adafruit/MCUFRIEND_kbv@^2.9.9
       adafruit/Adafruit TouchScreen@^1.1.1
     ```

   - Save the file; PlatformIO will automatically download the libraries.

5. **Build and Upload Code**

   - Click on **Build** to compile the code.
   - Click on **Upload** to upload the code to your Arduino board.

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

---

