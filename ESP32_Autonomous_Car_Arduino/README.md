# Setup Guide for Fedora Linux

To make this ESP32 Arduino code compile and upload flawlessly on Fedora Linux the first time, please follow these steps:

## 1. System Requirements & Permissions

Fedora uses `dialout` group to manage access to serial ports. By default, your user might not be in this group. You **must** add yourself to the dialout group to upload code to the ESP32.

Open your terminal and run:
`sudo usermod -aG dialout $USER`

**IMPORTANT**: Log out and log back in, or restart your computer for these permissions to take effect. If you skip this, the Arduino IDE will say "Permission denied" when accessing `/dev/ttyUSB0` or `/dev/ttyACM0`.

## 2. Install Arduino IDE

We recommend the official AppImage of the modern Arduino IDE 2.x for Fedora.

1. Download the latest Arduino IDE AppImage from the [official website](https://www.arduino.cc/en/software).
2. Assuming it downloads to your Downloads folder, make it executable:
   `chmod +x ~/Downloads/arduino-ide*.AppImage`
3. Run the AppImage:
   `~/Downloads/arduino-ide*.AppImage`

*(Alternatively, you can install via Flatpak `flatpak install cc.arduino.IDE2`, but Flatpak sometimes creates sandbox issues with serial ports unless specifically permitted).*

## 3. Install ESP32 Board Manager

1. Open Arduino IDE.
2. Go to **File -> Preferences**.
3. In the "Additional Boards Manager URLs" field, paste the following URL:
   `https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json`
4. Click OK.
5. Open the **Boards Manager** on the left icon bar (or via Tools -> Board -> Boards Manager).
6. Search for `esp32` by Espressif Systems and click **Install**.

## 4. Install Required Libraries

The code requires two external libraries. You can install them right inside the Arduino IDE.

1. Open the **Library Manager** on the left icon bar (or via Sketch -> Include Library -> Manage Libraries).
2. Search for and install:
   - **`ArduinoJson`** by Benoit Blanchon
   - **`ESP32Servo`** by Kevin Harrington, John K. Bennett

## 5. Compile and Upload

1. Open the `ESP32_Autonomous_Car_Arduino.ino` file in the Arduino IDE.
2. Under **Tools -> Board**, select your specific ESP32 model (usually "DOIT ESP32 DEVKIT V1" or "ESP32 Dev Module").
3. Connect your ESP32 via USB.
4. Go to **Tools -> Port** and ensure you select the port that appeared (likely `/dev/ttyUSB0`).
5. **Update your WiFi Credentials**: In the code, please remember to update your `ssid` and `password`. Also, if you need a different static IP scheme, edit the `local_IP` configuration.
6. Click the Right-Arrow **Upload** button to compile and flash the code!

Enjoy your translated Autonomous Car on Fedora!
