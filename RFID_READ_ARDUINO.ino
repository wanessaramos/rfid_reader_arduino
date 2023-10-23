#include <WiFi.h>
#include <HTTPClient.h>
#include <Wire.h>
#include <SPI.h>
#include <MFRC522.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define RST_PINO        26          // Configurable, see typical pin layout above
#define SS_PINO         27          // Configurable, see typical pin layout above

MFRC522 rfid(SS_PINO, RST_PINO);     // Instance of the class

#define SS_PIN    21
#define RST_PIN   22

#define SIZE_BUFFER     18
#define MAX_SIZE_BLOCK  16

#define SCREEN_WIDTH 128 // OLED width,  in pixels
#define SCREEN_HEIGHT 64 // OLED height, in pixels

// create an OLED display object connected to I2C
Adafruit_SSD1306 oled(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

const char* ssid = ""; //Replace the network name
const char* password = ""; // Replace password wifi
const char* serverAddress = "";  // Replace with your API address

String content= "";
String resp ="";

void setup() {
  Serial.begin(115200);
 
  // Connect to the Wi-Fi network
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Conectando ao WiFi...");
  }
  Serial.println("Conectado ao WiFi");

  SPI.begin();                      // Init SPI bus
  rfid.PCD_Init();                  // Init MFRC522

  // initialize OLED display with I2C address 0x3C
  if (!oled.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("failed to start SSD1306 OLED"));
    while (1);
  }

  delay(2000);         // wait two seconds for initializing
  oled.clearDisplay(); // clear display  
  
}

void loop() {
   // Look for new cards
  if ( ! rfid.PICC_IsNewCardPresent())
    return;

  // Verify if the NUID has been readed
  if ( ! rfid.PICC_ReadCardSerial())
    return;

  Serial.print("UID da tag :");
  content= "";
  for (byte i = 0; i < rfid.uid.size; i++) {
     Serial.print(rfid.uid.uidByte[i] < 0x10 ? " 0" : " ");
     Serial.print(rfid.uid.uidByte[i], HEX);
     content.concat(String(rfid.uid.uidByte[i] < 0x10 ? " 0" : " "));
     content.concat(String(rfid.uid.uidByte[i], HEX));
  }

  delay(500);

  if(content != ""){
    request(content);
  }

  display(resp);
}

void request(String tag){
  tag.replace(" ", "");
  // Create the URL with query parameters
  String url = serverAddress;
  url+=tag;

  // Initialize the HTTP connection
  HTTPClient http;
  http.begin(url);

  // Make a POST request to the API
  int httpCode = http.GET();

  Serial.println("HTTP CODE: " + httpCode);
 
  if (httpCode > 0) {
    String payload = http.getString();
    resp = "Acesso "+ payload;
    display(resp);
    resp = "";
  } else {
    Serial.println("Erro na solicitação HTTP");
  }
  http.end();
}

void display(String resp){
  oled.setTextSize(1);         // set text size
  oled.setTextColor(WHITE);    // set text color
  oled.setCursor(0, 10);       // set position to display

   if(resp == ""){
    oled.clearDisplay(); // clear display
    oled.println("Bem Vindo");
    oled.println("Aproxime o cartao"); // set text
    oled.display();
  }else{
    oled.clearDisplay(); // clear display
    oled.println(resp);
    oled.display();
    delay(1000);
  }
}





