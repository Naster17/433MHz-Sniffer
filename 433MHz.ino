/* 
  mySwitch.switchOn("11111", "00010");
  mySwitch.switchOff("11111", "00010");

  Same switch as above, but using decimal code // decimal + bits (standart: 24bit)
  mySwitch.send(5393, 24);
  mySwitch.send(5396, 24);

  mySwitch.send("000000000001010100010001"); // 24bit
  mySwitch.send("000000000001010100010100");

  // Хуй знает
  mySwitch.sendTriState("00000FFF0F0F");
  mySwitch.sendTriState("00000FFF0FF0");
*/
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <RCSwitch.h>

#ifndef APSSID
#define APSSID "iPhone"
#define APPSK "12345678"
#endif

/* Set these to your desired credentials. */
const char *ssid = APSSID;
const char *password = APPSK;

// Initialize RCSwitch objects for transmitting and receiving data on 433MHz
RCSwitch mySwitchTx;
RCSwitch mySwitchRx;

// Variable to store the received data
String receivedData;

// Initialize the AsyncWebServer object
AsyncWebServer server(80);

// Handler for the root URL
void handleRoot(AsyncWebServerRequest* request) {
  String html = "<html><body>";
  html += "<h2>Enter Data: </h2>";
  html += "<form action=\"/submit\" method=\"get\">";
  html += "<input type=\"text\" name=\"data\">";
  html += "<br>";
  html += "<p>Choise Type: </p>";
  html += "<input type='radio' name='option' value='1'>Binary   (000000000001010100010001)<br>";
  html += "<input type='radio' name='option' value='2'>TriState (00000FFF0F0F)<br>";
  html += "<input type=\"submit\" value=\"Send\">";
  html += "</form>";

  html += "<h2>Received Data: </h2>";
  html += "<ul>";
  html += receivedData;
  html += "</ul>";
  html += "</body></html>";
  request->send(200, "text/html", html);
}

// Handler for submitting data
void handleSubmit(AsyncWebServerRequest* request) {
  String data = request->arg("data");
  String option = request->arg("option");
  String type;
  if (option == "1")
   {
    Serial.println("1");
    type = "Binary";
    mySwitchTx.send(data.c_str());

  } else if (option == "2") 
  { 
    Serial.println("2");
    type = "TriState";
    mySwitchTx.sendTriState(data.c_str());

  } else 
  {
    type = "Error";
    Serial.println("Choise error");
  }
 
  Serial.println("Received Data: " + data);

  // Update the received data
  receivedData += "<li>SENDED: " + data + " Type: " + type + "</li>";

  request->redirect("/");
}


void setup() {
  Serial.begin(115200);

  Serial.println();
  Serial.print("Configuring access point...");
  WiFi.softAP(ssid, password);
  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);

  // Enable transmitting and receiving on the respective pins
  mySwitchTx.enableTransmit(D8);
  mySwitchRx.enableReceive(D2);

  // Set the default received data
  receivedData = "";

  // Route handlers
  server.on("/", HTTP_GET, handleRoot);
  server.on("/submit", HTTP_GET, handleSubmit);

  // Start the web server
  server.begin();

  Serial.println("Web server started!");
}



void loop() {
  // Check for new data on the receiver
  if (mySwitchRx.available()) {
    String result1, result2, result3, result4, result5, result6;
    output(mySwitchRx.getReceivedValue(), mySwitchRx.getReceivedBitlength(), mySwitchRx.getReceivedDelay(), mySwitchRx.getReceivedRawdata(), mySwitchRx.getReceivedProtocol(), &result1, &result2, &result3, &result4, &result5, &result6);

    mySwitchRx.resetAvailable();

    // Update the received data
    receivedData += "<li>RECIVED: Decimal: " + result1 + " Binary: " + result2 + " Tri-State: " + result3 + " Protocol: " + result4 + " Delay: " + result5 + " Lenght: " + result6 + "-bits </li>";
  }

  delay(100);
}

