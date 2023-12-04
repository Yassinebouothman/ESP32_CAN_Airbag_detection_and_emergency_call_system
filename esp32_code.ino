#include <SoftwareSerial.h>

SoftwareSerial gsmSerial(9, 10); //receive and transmit pins :: 9 => RX, 10 => TX

const char phoneNumber[] = "+21695593167"; 
const char simPIN[] = "xxxx";  // Sim card pin if you need it


void setup()
{

  gsmSerial.begin(9600); // Setting the baud rate of GSM Module
  Serial.begin(9600); // Setting the baud rate of Serial Monitor
  testSignal();
  delay(100);
  
}

void loop()
{

char* gpsVal = "4807.0380"; // example
SendMessage(phoneNumber, gpsVal);
DialCall(phoneNumber);

if (gsmSerial.available() > 0)
  {
    Serial.write(gsmSerial.read());
  }
}

/*
AT commands:
AT	              Checks if the module is responding.
AT+CPIN?          Checks the status of the SIM card (PIN ready or not).
AT+CSQ	          Checks the signal quality (measured in dBm).
AT+CREG?	        Checks the registration status with the network.
AT+CGATT?	        Checks if the device is attached to GPRS service.
AT+CIPSTATUS	    Gets the connection status with the server.
AT+CIPSTART	      Starts a TCP/UDP connection with the server.
AT+CIPSEND	      Sends data to the server in TCP/UDP connection.
AT+CIPCLOSE	      Closes the TCP/UDP connection with the server.
AT+CMGF=1	        Sets SMS text mode, allowing sending/receiving SMS in text format.
AT+CMGS=”number”	Initiates sending an SMS to the specified “number”.
AT+CMGR=index	    Reads an SMS from the SMS memory with the specified “index”.
*/

void testSignal()
{
  gsmSerial.println("AT");
   // Check signal strength
  gsmSerial.println("AT+CSQ");

  // Check if the module is registered on the network
  gsmSerial.println("AT+CREG?");

  // Check if the module is attached to the GPRS network
  gsmSerial.println("AT+CGATT?");

  // Check if the module is connected to the network
  gsmSerial.println("AT+CGATT?");
}


void SendMessage(const char* phoneNumber, char* message)
{
    gsmSerial.println("AT+CMGF=1"); // Sets the GSM Module in Text Mode
    delay(1000); // Delay of 1000 milliseconds or 1 second

    gsmSerial.print("AT+CMGS=\"");
    gsmSerial.print(phoneNumber);
    gsmSerial.println("\""); // Replace x with mobile number
    delay(100);

    gsmSerial.print("Coordonnées GPS de l'accident : "); // The SMS text you want to send
    delay(100);

    gsmSerial.println(message);
    delay(100);
}

void DialCall(const char* phoneNumber)
{
    Serial.println("Calling...");
    gsmSerial.print("ATD");
    gsmSerial.print(phoneNumber);
    gsmSerial.println(";"); // ATDxxxxxxxxxx; -- watch out here for semicolon at the end!!
    delay(100);
}