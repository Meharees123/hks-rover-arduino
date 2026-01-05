#include "arduino_secrets.h"
#include "esp_camera.h"
#include <HTTPClient.h>
#include <WiFi.h>

// Replace with your network credentials
const char *ssid = SECRET_SSID;
const char *password = SECRET_PASS;

// Replace with your server URL
const char *serverURL = "http://localhost:3000/api/camera/upload";

// Camera model - AI Thinker ESP32-CAM
#define PWDN_GPIO_NUM 32
#define RESET_GPIO_NUM -1
#define XCLK_GPIO_NUM 0
#define SIOD_GPIO_NUM 26
#define SIOC_GPIO_NUM 27
#define Y9_GPIO_NUM 35
#define Y8_GPIO_NUM 34
#define Y7_GPIO_NUM 39
#define Y6_GPIO_NUM 36
#define Y5_GPIO_NUM 21
#define Y4_GPIO_NUM 19
#define Y3_GPIO_NUM 18
#define Y2_GPIO_NUM 5
#define VSYNC_GPIO_NUM 25
#define HREF_GPIO_NUM 23
#define PCLK_GPIO_NUM 22

void setup() {
  Serial.begin(115200);
  Serial.println();

  // Initialize camera
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;

  config.frame_size = FRAMESIZE_QVGA;
  config.jpeg_quality = 20; // 0-63, lower means higher quality
  config.fb_count = 1;

  // Initialize camera
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }

  // Connect to WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("WiFi connected!");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  Serial.println("ESP32-CAM Ready!");
}

void loop() {
  // Capture image
  camera_fb_t *fb = esp_camera_fb_get();
  if (!fb) {
    Serial.println("Camera capture failed");
    delay(1000);
    return;
  }

  // Upload image
  if (uploadFrame(fb->buf, fb->len)) {
    Serial.println("Frame uploaded successfully");
  } else {
    Serial.println("Frame upload failed");
  }

  // Return frame buffer
  esp_camera_fb_return(fb);

  // Wait before next capture (adjust as needed)
  delay(200); // FPS
}

bool uploadFrame(uint8_t *buffer, size_t length) {
  HTTPClient http;

  String boundary = "----ESP32CAMBoundary1234567890";
  String contentType = "multipart/form-data; boundary=" + boundary;
  http.begin(serverURL);
  http.addHeader("Content-Type", contentType);

  // Multipart body
  String head = "--" + boundary + "\r\n";
  head += "Content-Disposition: form-data; name=\"frame\"; "
          "filename=\"frame.jpg\"\r\n";
  head += "Content-Type: image/jpeg\r\n\r\n";

  String tail = "\r\n--" + boundary + "--\r\n";

  size_t totalLength = head.length() + length + tail.length();
  uint8_t *payload = (uint8_t *)malloc(totalLength);

  if (!payload) {
    Serial.println("Failed to allocate memory for payload");
    return false;
  }

  // Copy everything into one payload
  memcpy(payload, head.c_str(), head.length());
  memcpy(payload + head.length(), buffer, length);
  memcpy(payload + head.length() + length, tail.c_str(), tail.length());

  // Send the request
  int httpResponseCode = http.sendRequest("POST", payload, totalLength);

  if (httpResponseCode > 0) {
    String response = http.getString();
    Serial.println("HTTP Response code: " + String(httpResponseCode));
    Serial.println("Response: " + response);
  } else {
    Serial.println("HTTP sendRequest failed: " + String(httpResponseCode));
  }

  http.end();
  free(payload);
  return httpResponseCode == 200;
}
