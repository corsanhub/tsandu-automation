#include <NTPClient.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

const char *ssid = "FerremundoNet";
const char *pass = "4431414478";

const char *deviceName = "LOCK_DRIVER_01";
const String deviceId = "LOCK-01";

const long utcOffsetInSeconds = 3600;

IPAddress staticIP(192, 168, 1, 90);  // ESP static ip
IPAddress gateway(192, 168, 1, 1);    // IP Address of your WiFi Router (Gateway)
IPAddress subnet(255, 255, 255, 0);   // Subnet mask
IPAddress dns(8, 8, 8, 8);            // DNS

const String OPEN = "OPEN";
const String CLOSE = "CLOSE";
const String SUCCESS = "SUCCESS";
const String FAILURE = "FAILURE";

WiFiServer server(80);
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);

String header;

unsigned long currentTime = millis();
unsigned long previousTime = 0;
const long timeoutTime = 3000;
char c;

void allOff() {
    // This is for the RAZA
    digitalWrite(D6, LOW);
}

void initialize() {
    Serial.print("Initializing data ports ...\n");
    pinMode(D6, OUTPUT);
    allOff();
}

String buildRequestString(String operation, String id, String action) {
    return "POST /operation=" + operation + "?id=" + id + "&action=" + action;
}

String buildResponse(String operation, String id, String action, String state) {
    return String("{'operation':'" + operation + "', 'id':'" + id + "', 'action':'" + action + "', 'state':'" + state + "'}");
}

String getCurrentTime(){
  unsigned long epochTime = timeClient.getEpochTime();
  struct tm *ptm = gmtime ((time_t *)&epochTime); 
  
  int year = ptm->tm_year + 1900;
  int month = ptm->tm_mon + 1;
  int day = timeClient.getDay();
  int hour = timeClient.getHours();
  int minute = timeClient.getMinutes();
  int second = timeClient.getSeconds() ;
  int milli = 0; //timeClient.getMillis();

  // Result string would be returned in yyyy-mm-dd hh:MM:ss.SSS format
  //String result = "" + year + "-" + month + "-" + day + " " + hour + ":" + minute + ":" + second + "." + milli;
  char dateStr[23];
  sprintf(dateStr, "%4d-%02d-%02d %02d:%02d:%02d.%03d", year, month, day, hour, minute, second, milli);
  return dateStr;
}

void setup() {
    initialize();

    // WiFi.persistent(false);
    Serial.begin(115200);
    Serial.printf("Connecting to network: %s ...\n", ssid);
    WiFi.hostname(deviceName);
    WiFi.config(staticIP, subnet, gateway, dns);
    WiFi.begin(ssid, pass);
    WiFi.mode(WIFI_STA);

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
    timeClient.begin();
}

void loop() {
    WiFiClient client = server.available();

    if (client) {                       // If a new client connects,
        Serial.println("New Client.");  // print a message out in the serial port
        String currentLine = "";        // make a String to hold incoming data from the client
        currentTime = millis();
        previousTime = currentTime;

        String operation = "CHANGE_STATUS";
        String LOCK_OPEN = buildRequestString(operation, deviceId, OPEN);
        String LOCK_CLOSE = buildRequestString(operation, deviceId, CLOSE);

        while (client.connected() && currentTime - previousTime <= timeoutTime) {  // loop while the client's connected
            currentTime = millis();
            if (client.available()) {    // if there's bytes to read from the
                                         // client,
                char c = client.read();  // read a byte, then
                Serial.write(c);         // print it out the serial monitor
                header += c;
                if (c == '\n') {  // if the byte is a newline character
                    if (currentLine.length() == 0) {
                        String statusOK =
                            "HTTP/1.1 200 OK\n"
                            "Content-type:application/json\n"
                            "Connection: close\n";
                        String statusERROR =
                            "HTTP/1.1 404 ERROR\n"
                            "Content-type:application/json\n"
                            "Connection: close\n";

                        String response = "";
                        if (header.indexOf(LOCK_OPEN) >= 0) {
                            digitalWrite(D6, HIGH);
                            delay(1200);
                            digitalWrite(D6, LOW);
                            response = statusOK + buildResponse(operation, deviceId, OPEN, SUCCESS);
                        } else if (header.indexOf(LOCK_CLOSE) >= 0) {
                            digitalWrite(D6, LOW);
                            response = statusOK + buildResponse(operation, deviceId, CLOSE, SUCCESS);
                        } else {
                            response = statusERROR + buildResponse(operation, deviceId, CLOSE, FAILURE);
                        }
                        timeClient.update();
                        String formattedTime = getCurrentTime();
                        
                        client.println(response + " {:header \"" + header + "\" :lock-open \"" + LOCK_OPEN + "\" :lock-close \"" + LOCK_CLOSE + "\" :driver-time \"" + formattedTime + "\" }\n");
                        client.println("");
                    }

                    break;
                } else {  // if you got a newline, then clear currentLine
                    currentLine = "";
                }
            } else if (c != '\r') {  // if you got anything else but a carriage return character,
                // client.printf("%c", c);
                currentLine += c;  // add it to the end of the currentLine
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
