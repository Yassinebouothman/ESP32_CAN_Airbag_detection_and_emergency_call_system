#include <TinyGPSPlus.h>
#include <SoftwareSerial.h>


SoftwareSerial gsmSerial(16, 17);  // RX, TX for GSM
SoftwareSerial gpsSerial(12, 14);  // RX, TX for GPS
TinyGPSPlus gps;

// variables initializationn
const char emergencyNumber[15] = "90550246";
const char familyNumber[15] = "90550246";
char emergencyMessage[100] = "";
char mapLink[100] = "";
char latitude[15] = "36.";   //36.
char longitude[15] = "10.";  //10.
char lat[15] = "";
char lng[15] = "";
bool gprsAvailable = false;
bool gsmAvailable = false;
String apn = "internet.tn";  //APN
String apn_u = "";           //APN-Username
String apn_p = "";
time_t timestamp;

String url = "http://sos-beacon-messages.000webhostapp.com/receive_message.php";

void setup() {
  Serial.begin(9600);
  gsmSerial.begin(9600);  // initialize gsm UART
  gpsSerial.begin(9600);  // initialize gps UART
  delay(500);
  testModule();  // testing gsm signal
  delay(5000);
  //getEmergencyMessage();
  gsm_config_gprs();
  //sendMessage(emergencyNumber, mapLink); // sending emergency message
  //Serial.println("Message sent");
}

void loop() {
  Check if GPS is available
  while (gpsSerial.available() > 0) {
    if (gps.encode(gpsSerial.read())) {
      if (gps.location.isValid()) {
        Serial.println("gps is valid");
        delay(1000);
        //displayInfo(); // displaying lat and long
        getEmergencyMessage();  // reading gps coordinates values and putting them in a clear message
      } else {
        Serial.println("Invalid GPS Data");
      }
    }
  }
  getEmergencyMessage();  // reading gps coordinates values and putting them in a clear message

  ATcommand("AT+CSQ");
  ATcommand("AT+CGATT?");
  //dialCall(emergencyNumber); // dialing emergency
  if (gprsAvailable) {
    gsm_http_post();
  }
  else if (gsmAvailable) {
    sendMessage(emergencyNumber, emergencyMessage);        // sending emergency message to civil protection containing gps location
    sendMessage(familyNumber, emergencyMessage);  // sending emergency message to family containing gps location
  }
  delay(1000);
}

/*
AT commands :
AT	                Checks if the module is responding.
AT+CPIN?	        Checks the status of the SIM card (PIN ready or not).
AT+CSQ	            Checks the signal quality (measured in dBm).
AT+CREG?	        Checks the registration status with the network.
AT+CGATT?	        Checks if the device is attached to GPRS service.
AT+CIPSTATUS	    Gets the connection status with the server.
AT+CIPSTART	        Starts a TCP/UDP connection with the server.
AT+CIPSEND	        Sends data to the server in TCP/UDP connection.
AT+CIPCLOSE	        Closes the TCP/UDP connection with the server.
AT+CMGF=1	        Sets SMS text mode, allowing sending/receiving SMS in text format.
AT+CMGS="number"	Initiates sending an SMS to the specified "number".
AT+CMGR=index	    Reads an SMS from the SMS memory with the specified "index".
*/

void testModule() {
  ATcommand("AT");
  ATcommand("AT+CSQ");
  //ATcommand("AT+CCID");
  //ATcommand("AT+CREG?");
  //ATcommand("ATI");
  //ATcommand("AT+CBC");
  ATcommand("AT+CGATT?");
  //ATcommand("AT+CFUN=1");
}

void sendMessage(const char* phoneNumber, char* message) {
  ATcommand("AT+CMGF=1");
  delay(1000);
  gsmSerial.print("AT+CMGS=\"");
  gsmSerial.print(phoneNumber);
  gsmSerial.println("\"");
  delay(100);
  ATcommand(message);
  gsmSerial.write(26);  // Ctrl+Z to send the message
  delay(100);
}

void dialCall(const char* phoneNumber) {
  Serial.println("Calling...");
  gsmSerial.print("ATD");
  gsmSerial.print(phoneNumber);
  gsmSerial.println(";");
  delay(100);
}


void getRandomData() {
  // Generate a random 4-digit number
  int randomNum1 = rand() % 9000 + 1000;  // Produces a number between 1000 and 9999
  int randomNum2 = rand() % 9000 + 1000;  // Produces a number between 1000 and 9999

  char randomStr1[5];  // One extra character for the null terminator '\0'
  char randomStr2[5];  // One extra character for the null terminator '\0'

  snprintf(randomStr1, sizeof(randomStr1), "%d", randomNum1);
  snprintf(randomStr2, sizeof(randomStr2), "%d", randomNum2);
  sprintf(lat, "%s%s", latitude, randomStr1);
  sprintf(lng, "%s%s", longitude, randomStr2);
  // Serial.print("latitude : ");
  // Serial.println(lat);
  // Serial.print("longitude : ");
  // Serial.println(lng);
}

void getEmergencyMessage() {
  // sprintf(latitude, "%.6f", gps.location.lat());
  // sprintf(longitude, "%.6f", gps.location.lng());
  // Serial.println("latitude: ");
  // Serial.println(latitude);
  // Serial.println("longitude: ");
  // Serial.println(longitude);
  getRandomData();
  sprintf(mapLink, "https://www.google.com/maps/@%s,%s,15z?entry=ttu", lat, lng);
  sprintf(emergencyMessage, "Accident detected on the location: Latitude : %s° N, Longitude : %s° W. This is the google maps location : %s", latitude, longitude, mapLink);
  Serial.print("Emergency Message is: ");
  Serial.println(emergencyMessage);
  delay(1000);
}

void displayInfo() {
  Serial.print(F("Location: "));
  Serial.print(gps.location.lat(), 6);
  Serial.print(F(","));
  Serial.print(gps.location.lng(), 6);
}

/*
AT+HTTPACTION=1 Error codes : 
600 : Not HTTP PDU
601 : Network Error
602 : No memory
603 : DNS Error
604 : Stack Busy
*/
void gsm_http_post() {
  getRandomData();  // reading gps coordinates values and putting them in a clear message
  Serial.println(" --- Start GPRS & HTTP --- ");
  String jsondata = "{\"latitude\":\"" + String(lat) + "\",\"longitude\":\"" + lng + "\"}";
  ATcommand("AT+SAPBR=1,1");
  ATcommand("AT+SAPBR=2,1");
  ATcommand("AT+HTTPINIT");
  ATcommand("AT+HTTPPARA=CID,1");
  ATcommand("AT+HTTPPARA=\"URL\",\"" + url + "\"");
  ATcommand("AT+HTTPPARA=CONTENT,application/json");
  ATcommand("AT+HTTPDATA=" + String(jsondata.length()) + ",10000");
  ATcommand(jsondata);
  ATcommand("AT+HTTPACTION=1");
  ATcommand("AT+HTTPREAD");
  ATcommand("AT+HTTPTERM");
  ATcommand("AT+SAPBR=0,1");
}

void gsm_config_gprs() {
  Serial.println(" --- CONFIG GPRS --- ");
  ATcommand("AT+SAPBR=3,1,Contype,GPRS");
  ATcommand("AT+SAPBR=3,1,APN," + apn);
  if (apn_u != "") {
    ATcommand("AT+SAPBR=3,1,USER," + apn_u);
  }
  if (apn_p != "") {
    ATcommand("AT+SAPBR=3,1,PWD," + apn_p);
  }
}

void ATcommand(String command) {
  Serial.println("Send ->: " + command);
  gsmSerial.println(command);

  unsigned long timeout = millis() + 5000;  // Set a timeout of 3000 milliseconds (3 seconds)

  while (millis() < timeout) {
    while (gsmSerial.available()) {
      String response = gsmSerial.readStringUntil('\n');
      Serial.println(response);

      // Check for specific responses and update flags accordingly
      if (response.indexOf("+CGATT: 1") != -1) {
        gprsAvailable = true;
      }
      if (response.indexOf("+CSQ:") != -1) {
        gsmAvailable = true;
      }
    }
  }

  Serial.println();
}
