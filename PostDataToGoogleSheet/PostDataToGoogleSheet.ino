// BY: Akshaya Niraula
// ON: Nov 11, 2016
// AT: http://www.embedded-lab.com/
/*
 * Code Originally found at http://embedded-lab.com/blog/post-data-google-sheets-using-esp8266/
 * 
 * BOARD: NodeMCU 1.0 (ESP-12E Module)
 * 
 * How to Program:
 * 1.) Diconnect the battery
 * 2.) Connect the USB to UART
 *     VCC <-> 3v3
 *     TX  <-> RX
 *     RX  <-> TX
 *     GND <-> GND
 * 3.) Slide switch to program
 * 4.) Press Reset Button
 * 5.) Click Upload
 * 
 * If you have trouble, restart the IDE, disconnect the USB and try again.
 */


#include <ESP8266WiFi.h>
#include "HTTPSRedirect.h"

#define DATA_POST_DELAY 900000

const char* ssid = "YOUUR-SSID";
const char* password = "YOUR-PASSWORD";




// The ID below comes from Google Sheets.
// Towards the bottom of this page, it will explain how this can be obtained
const char *GScriptId = "CHANGE-THIS-TO-YOUR SSCRIPTID";

// Push data on this interval
const int dataPostDelay = 900000;  // 15 minutes = 15 * 60 * 1000

const char* host = "script.google.com";
const char* googleRedirHost = "script.googleusercontent.com";

const int httpsPort =  443;
HTTPSRedirect client(httpsPort);

// Prepare the url (without the varying data)
String url = String("/macros/s/") + GScriptId + "/exec?";

const char* fingerprint = "F0 5C 74 77 3F 6B 25 D7 3B 66 4D 43 2F 7E BC 5B E9 28 86 AD";

// We will take analog input from A0 pin 
const int AnalogIn     = A0; 

void setup() {
  Serial.begin(115200);
  Serial.println("Connecting to wifi: ");
  Serial.println(ssid);
  Serial.flush();

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println(" IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println(" MAC address: ");
  Serial.println(WiFi.macAddress());

  
  Serial.print(String("Connecting to "));
  Serial.println(host);

  bool flag = false;
  for (int i=0; i<5; i++){
    int retval = client.connect(host, httpsPort);
    if (retval == 1) {
       flag = true;
       break;
    }
    else
      Serial.println("Connection failed. Retrying...");
  }

  // Connection Status, 1 = Connected, 0 is not.
  Serial.println("Connection Status: " + String(client.connected()));
  Serial.flush();
  
  if (!flag){
    Serial.print("Could not connect to server: ");
    Serial.println(host);
    Serial.println("Exiting...");
    Serial.flush();
    return;
  }

  // Data will still be pushed even certification don't match.
  if (client.verify(fingerprint, host)) {
    Serial.println("Certificate match.");
  } else {
    Serial.println("Certificate mis-match");
  }
}//end setup



// This is the main method where data gets pushed to the Google sheet
void postData(String tag, float value){
  if (!client.connected()){
    Serial.println("Connecting to client again..."); 
    client.connect(host, httpsPort);
  }
  String urlFinal = url + "tag=" + tag + "&value=" + String(value);
  client.printRedir(urlFinal, host, googleRedirHost);
}// end postData




// Continue pushing data at a given interval
void loop() {
  
  //Read the sensor 10 times
  int numReads = 10;
  float dataRead[numReads];
  
  for (int i=0; i<numReads; i++){
    dataRead[i] = analogRead(AnalogIn);
  }
  float temp;

  //sort the temp readings from lowest to highest
  for (int i=0; i<numReads - 1; i++){
    if (dataRead[i] > dataRead[i + 1]){
      temp = dataRead[i];
      dataRead[i] = dataRead[i + 1];
      dataRead[i + 1] = temp;
    }
  }

  //average all but the highest and lowest reading
  float averageTemp = 0;
  
  for (int i=1; i<numReads - 1; i++){
    averageTemp += dataRead[i];
  }

  //compute the average
  averageTemp /= (numReads - 2);
  
  // Read analog value, in this case a soil moisture
  //int data = 1023 - analogRead(AnalogIn);
  //float data = ((int) (averageTemp * 100 ))/100.0;

  //resistors that make the voltage divider, 
  // the gain is needed to rescale the analog read
  float r1 =100.0;
  float r2 = 50.0;
  float gb = r2/(r1+r2);

  //analog scale 0V = 0; 1V = 1024
  //analog pin is 1V tollerant
  float analog = 1/1024.0;

  //Voltage out from the sensor
  // Scaled
  float V_O = averageTemp * analog;
  //temperature equation from the datasheet
  // the gb is the gain
  float T = -1481.96 + sqrt(2.1962E+6 + (1.8639 - V_O/gb)/3.88E-6);                        
  //data = T;
  // Post these information
  postData(WiFi.macAddress(), T);

  String s = "\t averageTemp = " + String(averageTemp) + "\n\t gb = " + String(gb) + "\n\t analog = " + String(analog) + "\n\t V_O = " + String(V_O) + "\n\t T = " + String(T);
  Serial.println(s);
  //deep sleep in milliseconds
  ESP.deepSleep(3720e6);
  //delay (dataPostDelay);
}// end loop
