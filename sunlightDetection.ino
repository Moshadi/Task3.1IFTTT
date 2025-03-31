#include <WiFiNINA.h>
#include <Wire.h>
#include <BH1750.h>

char ssid[] = "iPhone";       
char pass[] = "Moshi@2000";    

char HOST_NAME[] = "maker.ifttt.com";
String EVENT_START = "sunlight_detected";   
String EVENT_STOP  = "sunlight_gone";      
String IFTTT_KEY   = "plkNbtVb7nTfaPjNqYUWcb8YzhGzTcmsEejQ1Ito7sP";   

WiFiClient client;
BH1750 lightSensor;

bool sunlightDetected = false;
int threshold = 300;  

void setup() {
  Serial.begin(9600);
  while (!Serial);

  // Connect to WiFi
  WiFi.begin(ssid, pass);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected to WiFi");

  Wire.begin();
  if (lightSensor.begin()) {
    Serial.println("BH1750 sensor started");
  } else {
    Serial.println("Error starting BH1750 sensor");
    while (1); 
  }
}

void loop() {
  float lux = lightSensor.readLightLevel();
  Serial.print("Light: ");
  Serial.print(lux);
  Serial.println(" lux");

  // Check sunlight condition and send to IFTTT
  if (lux > threshold && !sunlightDetected) {
    sendToIFTTT(EVENT_START, lux);  // Trigger sunlight detection
    sunlightDetected = true;
  } else if (lux < threshold && sunlightDetected) {
    sendToIFTTT(EVENT_STOP, lux);   // Trigger sunlight stop
    sunlightDetected = false;
  }

  delay(5000);  // Wait for 5 seconds before reading again
}

void sendToIFTTT(String eventName, float lightLevel) {
  String path = "/trigger/" + eventName + "/with/key/" + IFTTT_KEY;
  String query = "?value1=";

  // Make the GET request to IFTTT
  if (client.connect(HOST_NAME, 80)) {
    Serial.println("Connected to IFTTT");

    client.println("GET " + path + query + " HTTP/1.1");
    client.println("Host: " + String(HOST_NAME));
    client.println("Connection: close");
    client.println();

    // Read and print response from IFTTT (optional for debugging)
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        Serial.print(c);
      }
    }

    client.stop();  // Disconnect after request
    Serial.println("Disconnected from IFTTT");
  } else {
    Serial.println("Connection to IFTTT failed");
  }
}
