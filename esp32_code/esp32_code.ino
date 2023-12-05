#include <TinyGPSPlus.h>
#include <SoftwareSerial.h>


SoftwareSerial gsmSerial(34, 35); // RX, TX for GSM
SoftwareSerial gpsSerial(16, 17);   // RX, TX for GPS
TinyGPSPlus gps;

// variables initializationn
const char emergencyNumber[15] = "+21695593167";
const char familyNumber[15] = "+21695593167";
char emergencyMsg[50] = "";
char latitude[15] = "";
char longitude[15] = "";

void setup()
{
    Serial.begin(9600);
    gsmSerial.begin(9600); // initialize gsm UART
    gpsSerial.begin(9600); // initialize gps UART
    while(!gsmSerial.available()) 
    {
      Serial.print("Connecting to gsm UART")
      Serial.print(".");
      delay(500);
    }
    Serial.println("GSM is connected successfully");
    delay(3000);
    testSignal();  // testing gsm signal
    delay(100);
}

void loop()
{

      // Check if GSM data is available
    while (gsmSerial.available() > 0)
    {
        Serial.write(gsmSerial.read());
    }

    // Check if GPS is available
    while (gpsSerial.available() > 0)
    {
        if (gps.encode(gpsSerial.read()))
        {
          if (gps.location.isValid())
          {
            displayInfo(); // displaying lat and long
            getEmergencyMsg(); // reading gps coordinates values and putting them in a clear message
          }
        }
    }

    dialCall(emergencyNumber); // dialing emergency
    sendMessage(emergencyNumber, emergencyMsg); // sending emergency message to civil protection containing gps location
    sendMessage(familyNumber, emergencyMsg); // sending emergency message to family containing gps location
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

void testSignal()
{
    gsmSerial.println("AT");
    gsmSerial.println("AT+CSQ");
    gsmSerial.println("AT+CREG?");
    gsmSerial.println("AT+CGATT?");
    gsmSerial.println("AT+CGATT?");
}

void sendMessage(const char* phoneNumber, char* message)
{
    gsmSerial.println("AT+CMGF=1");
    delay(1000);

    gsmSerial.print("AT+CMGS=\"");
    gsmSerial.print(phoneNumber);
    gsmSerial.println("\"");
    delay(100);

    gsmSerial.println(message);
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

void getEmergencyMsg()
{
  sprintf(latitude, "%.6f", gps.location.lat());
  sprintf(longitude, "%.6f", gps.location.lng());
  sprintf(emergencyMsg, "Accident detected on the location: Latitude : %s° N, Longitude : %s° W", latitude, longitude);
  Serial.print("Emergency Message is: ");
  Serial.println(emergencyMsg);
}

void displayInfo()
{
    Serial.print(F("Location: "));
    Serial.print(gps.location.lat(), 6);
    Serial.print(F(","));
    Serial.print(gps.location.lng(), 6);
}
