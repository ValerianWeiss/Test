
#include <Constants.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h> 
#include <ESP8266WebServer.h>


/* Set these to your desired credentials. */
const char *ssid = "ESP-ap";
const char *password = "test123";

ESP8266WebServer server(80);
int activeFlag = SESSION_NOT_ACTIVE;
String message = "";

/* Just a little test message.  Go to http://192.168.4.1 in a web browser
 * connected to this access point to see it.
 */
void handleRoot() {
  server.send(200, "text/html", "<h1>You are connected</h1>");
}

String readFromSerial(){
   String recievedStr = "";
  //Reading data from arduino
  Serial.print("start reading from arduino");
  while(!recievedStr.endsWith("}\n")){
	  recievedStr += Serial.readString();
  }
  Serial.print("ESP recieved String: " + recievedStr);
  return recievedStr;
}

bool sendJsonResponseToClient(String message = message){    
  //Send data from arduino to Client
  if(message == ""){
    //invalid json message
    server.send(200, "application/json", "{\"status\":\"-1\"}");
    return false;
  }
  server.send(200, "application/json", message);  
  return true;
}

bool sendToArduino(){
  if(server.hasArg("plain") != true){
      server.send(200, "application/json", "{\"status\":\"-1\"}");
      return false;
  }
  Serial.print(server.arg("plain"));
  Serial.print("\n");
  //the response which is getting produced and send by to arduino
  if(!sendJsonResponseToClient(readFromSerial())){
      return false;
  }
  return true;
}

void handleStartBikesession(){
  if(activeFlag != SESSION_ACTIVE){  
    if(sendToArduino()){
     activeFlag = SESSION_ACTIVE; 
    }
  }else{
    server.send(200, "application/json", "{\"status\":\"-3\"}");
  }
}

void handleGetData(){
  sendJsonResponseToClient();
}

void handleStop(){
  if(activeFlag != SESSION_NOT_ACTIVE){
    sendToArduino();
    activeFlag = SESSION_NOT_ACTIVE;      
  }else{
    server.send(200,"application/json", "{\"status\":\"-3\"}");
  }  
}

void setup() {
  delay(1000);
  Serial.begin(115200);
  //Serial.println();
  //Serial.print("Configuring access point...");
  
  /* You can remove the password parameter if you want the AP to be open. */
  IPAddress myIP = WiFi.softAP(ssid, password);
  
  //Serial.print("AP IP address: ");
  //Serial.println(myIP);
  server.on("/", handleRoot);
  server.on("/stop", handleStop);  
  server.on("/startBikeSession", handleStartBikesession);
  server.on("/getData",  handleGetData);
  server.begin();
  //Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();
  if(activeFlag == SESSION_ACTIVE){
    message = readFromSerial();
  }
}

