#include <WiFi.h>
#include <PubSubClient.h>

// ตั้งค่า WiFi ของคุณ
const char* ssid = "phan";
const char* password = "plam12345";

// ตั้งค่า MQTT Broker
const char* mqtt_server = "broker.hivemq.com";
const int mqtt_port = 1883;

// กำหนดขาพินเซนเซอร์
const int gasPin = 34;     // MQ-2 Gas Sensor (Analog)
const int flamePin = 35;   // Flame Sensor

WiFiClient espClient;
PubSubClient client(espClient);

void setup_wifi() {
    delay(10);
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssid);

    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nWiFi connected");
}

void reconnect() {
    while (!client.connected()) {
        Serial.print("Attempting MQTT connection...");
        if (client.connect("ESP32FireClient")) {
            Serial.println("connected");
        } else {
            Serial.print("failed, rc=");
            Serial.print(client.state());
            Serial.println(" try again in 5 seconds");
            delay(5000);
        }
    }
}

void setup() {
    Serial.begin(115200);
    pinMode(gasPin, INPUT);
    pinMode(flamePin, INPUT);

    setup_wifi();
    client.setServer(mqtt_server, mqtt_port);
}

void loop() {
    if (!client.connected()) {
        reconnect();
    }
    client.loop();

    // อ่านค่าจากเซนเซอร์
    int gasValue = analogRead(gasPin);
    int flameValue = digitalRead(flamePin);

    // สร้างข้อความรูปแบบ JSON ส่งเข้า MQTT Topic (แก้ไขเครื่องหมายคำพูดให้ถูกต้องแล้ว)
    String payload = "{\"gas\":" + String(gasValue) + ", \"flame\":" + String(flameValue) + "}";
    client.publish("esp32/fire_sensor", payload.c_str());

    // แสดงผลทาง Serial Monitor เพื่อตรวจสอบค่า
    Serial.print("Published -> Gas: ");
    Serial.print(gasValue);
    Serial.print(" | Flame: ");
    Serial.println(flameValue);

    delay(2000); // ส่งข้อมูลทุกๆ 2 วินาที
}