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
          if (currentLine.length() == 0) {

            String statusOK = "HTTP/1.1 200 OK\nContent-type:application/json\nConnection: close\n";
            String statusERROR = "HTTP/1.1 404 ERROR\nContent-type:application/json\nConnection: close\n";

            String response = "";

            if (header.indexOf("POST /op=cs?id=L-G001&action=on") >= 0) {
              digitalWrite(D6, HIGH);
              response = statusOK + "{'operation':'changeStatus', 'elementId':'L-G001', 'action':'on', 'result':'success'}";
            } else if (header.indexOf("POST /op=cs?id=L-G001&action=off") >= 0) {
              digitalWrite(D6, LOW);
              response = statusOK + "{'operation':'changeStatus', 'elementId':'L-G001', 'action':'off', 'result':'success'}";
            } else if (header.indexOf("POST /op=cs?id=L-Y001&action=on") >= 0) {
              digitalWrite(D7, HIGH);
              response = statusOK + "{'operation':'changeStatus', 'elementId':'L-Y001', 'action':'on', 'result':'success'}";
            } else if (header.indexOf("POST /op=cs?id=L-Y001&action=off") >= 0) {
              digitalWrite(D7, LOW);
              response = statusOK + "{'operation':'changeStatus', 'elementId':'L-Y001', 'action':'off', 'result':'success'}";
            } else if (header.indexOf("POST /op=cs?id=L-R001&action=on") >= 0) {
              digitalWrite(D8, HIGH);
              response = statusOK + "{'operation':'changeStatus', 'elementId':'L-R001', 'action':'on', 'result':'success'}";
            } else if (header.indexOf("POST /op=cs?id=L-R001&action=off") >= 0) {
              digitalWrite(D8, LOW);
              response = statusOK + "{'operation':'changeStatus', 'elementId':'L-R001', 'action':'off', 'result':'success'}";
            } else {
              response = statusERROR + "{'operation':'changeStatus', 'elementId':'L-001', 'result':'failure'}";
            }
            client.println(response + "\n");
            client.println("");
          }

          break;
        } else { // if you got a newline, then clear currentLine
          currentLine = "";
        }
      } else if (c != '\r') {  // if you got anything else but a carriage return character,
          //client.printf("%c", c);
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
