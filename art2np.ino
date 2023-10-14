#include <SPI.h>
#include <M5_Ethernet.h>
#include <EthernetUdp.h>
#include <Adafruit_NeoPixel.h>

#define SCK  22
#define MISO 23
#define MOSI 33
#define CS   19

#define ARTNET_PORT 6454

#define NEOPIXEL_PIN 27
#define NUMPIXELS 1

Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUMPIXELS, NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800);

byte mac[] = {0x4C, 0x75, 0x25, 0xCE, 0xFE, 0x68 };

IPAddress ip(192, 168, 1, 177);
IPAddress subnet(255, 255, 255, 0);
IPAddress gateway(192, 168, 1, 1);
IPAddress dns(192, 168, 1, 1);

EthernetUDP Udp;

void setup() {
    SPI.begin(SCK, MISO, MOSI, -1);
    Ethernet.init(CS);

    Serial.begin(115200);
    while (!Serial) {
        delay(10);
    }

    strip.begin();
    strip.setBrightness(255);
    strip.show(); 

    if (Ethernet.begin(mac) == 0) {
        Serial.println("Failed to configure Ethernet using DHCP");
        Serial.println("Trying to use static IP address...");
        Ethernet.begin(mac, ip, dns, gateway, subnet);
    }
    Udp.begin(ARTNET_PORT);
    Serial.print("Connected to network! IP address: ");
    Serial.println(Ethernet.localIP());
}

void loop() {
    int packetSize = Udp.parsePacket();
    if (packetSize) {
        byte buffer[packetSize];
        Udp.read(buffer, packetSize);
        if (isArtnetPacket(buffer)) {
            // Serial.println("Received Art-Net packet");

            // Set NeoPixel color using DMX data
            int red = buffer[18];   // 1st channel
            int green = buffer[19]; // 2nd channel
            int blue = buffer[20];  // 3rd channel
            strip.setPixelColor(0, strip.Color(red, green, blue));
            strip.show();

            // DMXデータを配列として表示 (コメントアウト)
            /*
            Serial.print("[");
            for(int i = 0; i < packetSize - 18 - 1; i++) {
                Serial.print(buffer[18 + i], DEC);
                Serial.print(", ");
            }
            Serial.print(buffer[18 + packetSize - 18 - 1], DEC);
            Serial.println("]");
            Serial.println("------");
            */
        }
    }
}

bool isArtnetPacket(byte* buffer) {
    return strncmp((char*)buffer, "Art-Net\0", 8) == 0;
}
