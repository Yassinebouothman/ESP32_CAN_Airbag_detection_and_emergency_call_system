#include <TinyGPSPlus.h>
#include <SoftwareSerial.h>


SoftwareSerial gsmSerial(16, 17); // RX, TX for GSM
SoftwareSerial gpsSerial(9, 10);   // RX, TX for GPS
TinyGPSPlus gps;

// variables initializationn
const char emergencyNumber[15] = "97718240";
const char familyNumber[15] = "97718240";
char emergencyMessage[100] = "";
char latitude[15] = "1489.444";
char longitude[15] = "1489.444";

String apn = "internet.tn";                    //APN
String apn_u = "";                     //APN-Username
String apn_p = "";  

String url = "http://sos-beacon-messages.000webhostapp.com/receive_message.php";

void setup()
{
    Serial.begin(9600);
    gsmSerial.begin(9600); // initialize gsm UART
    gpsSerial.begin(9600); // initialize gps UART
    delay(1000);
    testModule();  // testing gsm signal
    delay(1000);
    gsm_config_gprs();
}

void loop()
{
    //Check if GPS is available
    while (gpsSerial.available() > 0)
    {
        if (gps.encode(gpsSerial.read()))
        {
          if (gps.location.isValid())
          {
            displayInfo(); // displaying lat and long
            getEmergencyMessage(); // reading gps coordinates values and putting them in a clear message
          }
          else 
          {
            Serial.println("Invalid GPS Data");
          }
        }
    }
    ATcommand("AT+CSQ");
    ATcommand("AT+CGATT?");

    delay(1000);
    //dialCall(emergencyNumber); // dialing emergency
    //getEmergencyMessage();
    //  if (gsmSerial.find("+CGATT: 1"))
    // {
    gsm_http_post("hello from GSM");

    // }    
    //  else if (gsmSerial.find("+CSQ:"))
    //  {
      //sendMessage(emergencyNumber, emergencyMessage); // sending emergency message to civil protection containing gps location
      //sendMessage(familyNumber, emergencyMessage); // sending emergency message to family containing gps location
      Serial.println("Message sent");
    // }
    delay(10000);

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

void testModule()
{
  ATcommand("AT");
  ATcommand("AT+CSQ");
  ATcommand("AT+CCID");
  ATcommand("AT+CREG?");
  ATcommand("ATI");
  ATcommand("AT+CBC");
  ATcommand("AT+CGATT?");
  ATcommand("AT+CFUN=1");

}

void sendMessage(const char* phoneNumber, char* message)
{
    ATcommand("AT+CMGF=1");
    delay(1000);
    gsmSerial.print("AT+CMGS=\"");
    gsmSerial.print(phoneNumber);
    gsmSerial.println("\"");
    delay(100);
    ATcommand(message);
    gsmSerial.write(26); // Ctrl+Z to send the message
    delay(100);
}

void dialCall(const char* phoneNumber)
{
    Serial.println("Calling...");
    gsmSerial.print("ATD");
    gsmSerial.print(phoneNumber);
    gsmSerial.println(";");
    delay(100);
}

void getEmergencyMessage()
{
  // sprintf(latitude, "%.6f", gps.location.lat());
  // sprintf(longitude, "%.6f", gps.location.lng());
  sprintf(emergencyMessage, "Accident detected on the location: Latitude : %s° N, Longitude : %s° W", latitude, longitude);
  //Serial.print("Emergency Message is: ");
  //Serial.println(emergencyMessage);
}

void displayInfo()
{
    Serial.print(F("Location: "));
    Serial.print(gps.location.lat(), 6);
    Serial.print(F(","));
    Serial.print(gps.location.lng(), 6);
}

void gsm_http_post(String postdata) {
  Serial.println(" --- Start GPRS & HTTP --- ");
  unsigned long timestamp = millis(); // You can use other methods to get a timestamp
  String jsondata = "{\"timestamp\":" + String(timestamp) + ",\"message\":\"" + postdata + "\"}";
  ATcommand("AT+SAPBR=1,1");
  delay(5000);
  ATcommand("AT+SAPBR=2,1");
  delay(5000);
  ATcommand("AT+HTTPINIT");
  delay(10000);
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
  long wtimer = millis();
  while (wtimer + 3000 > millis()) {
    while (gsmSerial.available()) {
      Serial.write(gsmSerial.read());
    }
  }
  Serial.println();
}
