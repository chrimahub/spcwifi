/*
 * SPC WiFi Remote Control Board (WeMos D1 R2) v1.0
 * SPCWiFi.cpp
 * Christian Masas
 * 5/2/2018
 * Senior Project Development
 * 
 * Special Notes:
 * The ESP8266 stores the values of the previously connected 
 * network in EEPROM as c type strings:
 * WiFi.SSID().c_str()
 * WiFi.psk().c_str()
 */
#include <Arduino.h>      // standard arduino library
#include <ESP8266WiFi.h>  // wifi functions library
#define W1 D5

// global variables
const char* network = WiFi.SSID().c_str();
const char* key = WiFi.psk().c_str();
WiFiServer server(80);

/*
 * Reconnects to my WiFi router using the previously stored 
 * connection values.
 */
void reconnect()
{
  // initial connection message
  Serial.println();
  Serial.print("connecting...");

  // startup the wifi using previously stored connection values
  WiFi.begin(network, key);

  // while connection is being made print more dots
  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(300);
    Serial.print(".");
  } // end while

  // connection successful message
  Serial.println("");
  Serial.println("wifi connected.");
} // end reconnect

/*
 * Displays information about the network currently connected 
 * to and the IP and MAC address of the WiFi shield.
 */
void displayNetwork()
{
  // show ssid
  Serial.print("ssid: ");
  Serial.println(WiFi.SSID());

  // show connected routers mac address
  //Serial.print("mac: ");
  //Serial.println(WiFi.BSSIDstr());

  // show rssi
  //Serial.print("rssi: ");
  //Serial.println(WiFi.RSSI());

  // show encryption type
  //Serial.print("enc: ");
  //Serial.println((WiFi.encryptionType(0) == ENC_TYPE_NONE)?"unsecured":"secured");
 
  // show mac address
  //Serial.print("\nd1 mac: ");
  //Serial.println(WiFi.macAddress());

  // show ip address we will need this for our client remote control
  Serial.print("d1 ip address: ");
  Serial.println(WiFi.localIP());
} // end displayNetwork

/*
 * This functions sets up an HTTP client remote control over 
 * pin D5. Once the server becomes available and the client sends 
 * some data the request is read. After matching the request, the 
 * response is returned and control is handed over to the html page 
 * of the viewing device (in this case my phone).
 */
void clientControl()
{
  // client check
  WiFiClient client = server.available();
  if (!client) 
  {
    return;
  } // end if

  // wait for client to send something
  Serial.println("new client.");
  while(!client.available())
  {
    delay(1);
  } // end while
 
  // read the request
  String request = client.readStringUntil('\r');  // read until char return
  Serial.println(request);

  // we dont want client info in our tcp/ip stack
  client.flush();
 
  // match the request
  int value = LOW;
  if (request.indexOf("/CONTROL=ON") != -1) 
  {
    digitalWrite(W1, HIGH);
    value = HIGH;
  } // end if
  
  if (request.indexOf("/CONTROL=OFF") != -1)
  {
    digitalWrite(W1, LOW);
    value = LOW;
  } // end if
  
  // return the response as a standard html page
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  // DO NOT DELETE THE NEXT LINE
  client.println(""); 
  client.println("<!DOCTYPE HTML>");
  client.println("<html>");
  client.print("Device is now: ");

  // change the status text based on the value
  if (value == HIGH) 
  {
    client.print("On");  
  } 
  else 
  {
    client.print("Off");
  } // end if

  // reference links for control usage
  client.println("<br><br>");
  client.println("Turn <a href=\"/CONTROL=ON\">ON</a><br>");
  client.println("Turn <a href=\"/CONTROL=OFF\">OFF</a><br>");
  client.println("</html>");
  delay(1);

  // prints disconnect message  
  Serial.println("client disconnected.");
  Serial.println("");
} // end clientControl

/*
 * Sketch setup includes pinmodes, wifi connect and display functions, and 
 * serial monitoring.
 */
void setup() 
{
  // serial monitor for debugging
  Serial.begin(9600);
  delay(1);

  // set pinmode for remote control use
  pinMode(W1, OUTPUT);
  digitalWrite(W1, LOW);
  
  // reconnect to wifi and display network info
  reconnect();
  displayNetwork();
 
  // start the control server
  server.begin();
  Serial.println("server started.");
  
  Serial.println("setup done.");
} // end setup

/*
 * Functions placed here will be looped forever.
 */
void loop() 
{
  clientControl();    // start client control
} // end loop
