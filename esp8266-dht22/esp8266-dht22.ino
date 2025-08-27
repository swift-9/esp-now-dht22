#include <ESP8266WiFi.h>
#include <espnow.h>
#include <DHT.h>

#define DHTPIN D4
#define DHTTYPE DHT22

DHT dht(DHTPIN, DHTTYPE);

uint8_t esp32Address[] = {0x68, 0x25, 0xDD, 0x32, 0x0F, 0x7C}; // Replace with ESP32 MAC

typedef struct struct_message {
  float temperature;
  float humidity;
} struct_message;

struct_message data;

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  Serial.print("ESP8266 MAC: "); Serial.println(WiFi.macAddress());

  if (esp_now_init() != 0) {
    Serial.println("ESP-NOW init failed!");
    return;
  }

  esp_now_set_self_role(ESP_NOW_ROLE_CONTROLLER);
  esp_now_add_peer(esp32Address, ESP_NOW_ROLE_SLAVE, 1, NULL, 0);

  dht.begin();
}

void loop() {
  data.temperature = dht.readTemperature();
  data.humidity = dht.readHumidity();
  

  if (!isnan(data.temperature) && !isnan(data.humidity)) {
    esp_now_send(esp32Address, (uint8_t *) &data, sizeof(data));
    Serial.printf("Sent -> Temp: %.2f °C | Hum: %.2f %%\n", data.temperature, data.humidity);
  } else {
    Serial.println("Failed to read from DHT22 sensor!");
  }

  delay(2000);
}
