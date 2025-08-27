#include <WiFi.h>
#include <esp_now.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

typedef struct struct_message {
  float temperature;
  float humidity;
} struct_message;

struct_message incomingData;

void onReceive(const uint8_t *mac, const uint8_t *incomingDataBytes, int len) {
  memcpy(&incomingData, incomingDataBytes, sizeof(incomingData));
  Serial.printf("Received -> Temp: %.2f °C | Hum: %.2f %%\n", incomingData.temperature, incomingData.humidity);

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 10);
  display.printf("Temp: %.2f C", incomingData.temperature);
  display.setCursor(0, 30);
  display.printf("Hum : %.2f %%", incomingData.humidity);
  display.display();
}

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  Serial.print("ESP32 MAC: "); Serial.println(WiFi.macAddress());

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("SSD1306 allocation failed");
    for(;;);
  }
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("Waiting for data...");
  display.display();

  if (esp_now_init() != ESP_OK) {
    Serial.println("ESP-NOW init failed!");
    return;
  }

  esp_now_register_recv_cb(onReceive);
}

void loop() {
  // Nothing here, all handled by callback
}
