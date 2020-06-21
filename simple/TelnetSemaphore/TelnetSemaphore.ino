#include <ESP8266WiFi.h>

const char *ssid = "gema_network";
const char *pass = "K@ptz1Nis1mA";

WiFiServer server(80);

String header;

unsigned long currentTime = millis();
unsigned long previousTime = 0; 
const long timeoutTime = 3000;
char c;

void allOff(){
  digitalWrite(D6, LOW);
  digitalWrite(D7, LOW);
  digitalWrite(D8, LOW);
}
void initialize() {
  Serial.print("Initializing data ports ...\n");

  pinMode(D6, OUTPUT);
  pinMode(D7, OUTPUT);
  pinMode(D8, OUTPUT);

  allOff();
}

void setup() {
  initialize();

  //WiFi.persistent(false);
  Serial.begin(115200);
  Serial.printf("Connecting to network: %s ...\n", ssid);
  WiFi.begin(ssid, pass);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  // Print local IP address and start web server
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  server.begin();
}

void loop() {
  WiFiClient client = server.available();

  if (client) {                             // If a new client connects,
    Serial.println("New Client.");          // print a message out in the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    currentTime = millis();
    previousTime = currentTime;
    while (client.connected() && currentTime - previousTime <= timeoutTime) { // loop while the client's connected
      currentTime = millis();         
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        header += c;
        if (c == '\n') {                    // if the byte is a newline character
          client.printf("header: [%s]", header.c_str());
          if (header.indexOf("GN") >= 0) {
              client.println("Turning Green LED state to [ON] ...");
              digitalWrite(D6, HIGH);
          } else if (header.indexOf("GF") >= 0) {
              client.println("Turning Green LED state to [ON] ...");
              digitalWrite(D6, LOW);
          } else if (header.indexOf("YN") >= 0) {
              client.println("Turning Yellow LED state to [ON] ...");
              digitalWrite(D7, HIGH);
          } else if (header.indexOf("YF") >= 0) {
              client.println("Turning Yellow LED state to [OFF] ...");
              digitalWrite(D7, LOW);
          } else if (header.indexOf("RN") >= 0) {
              client.println("Turning Red LED state to [ON] ...");
              digitalWrite(D8, HIGH);
          } else if (header.indexOf("RF") >= 0) {
              client.println("Turning Red LED state to [OFF] ...");
              digitalWrite(D8, LOW);
          }

          client.println("####");
          // Break out of the while loop
          break;
        } else { // if you got a newline, then clear currentLine
          currentLine = "";
        }
      } else if (c != '\r') {  // if you got anything else but a carriage return character,
          client.printf("%c", c);
          currentLine += c;      // add it to the end of the currentLine
      }
    }
  }

  // Clear the header variable
  header = "";
  // Close the connection
  client.stop();
  Serial.println("Client disconnected.");
  Serial.println("");
}
