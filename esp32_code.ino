#include <Arduino.h>
#include <SoftwareSerial.h>
#include <TinyGPS++.h>

SoftwareSerial gsmSerial(34, 35); // RX, TX for GSM
SoftwareSerial gpsSerial(16, 17);   // RX, TX for GPS
TinyGPSPlus gps;

const char phoneNumber[] = "+21695593167";

void setup()
{
    Serial.begin(9600);
    gsmSerial.begin(9600);
    gpsSerial.begin(9600);
    delay(3000);
    testSignal();
    delay(100);
}

void loop()
{
    // Check if GPS data is available
    while (gpsSerial.available() > 0)
    {
        if (gps.encode(gpsSerial.read()))
        {
            displayInfo();
            sendGPSData(phoneNumber, gps.location.lat(), gps.location.lng());
        }
    }

    // Check if GSM data is available
    while (gsmSerial.available() > 0)
    {
        Serial.write(gsmSerial.read());
    }
}




void sendGPSData(const char *phoneNumber, float latitude, float longitude)
{
    char* message;
    sprintf(message, "GPS Coordinates: %.6f, %.6f", latitude, longitude);

    SendMessage(phoneNumber, message);
    DialCall(phoneNumber);
}

void testSignal()
{
    gsmSerial.println("AT");
    gsmSerial.println("AT+CSQ");
    gsmSerial.println("AT+CREG?");
    gsmSerial.println("AT+CGATT?");
    gsmSerial.println("AT+CGATT?");
}

void SendMessage(const char *phoneNumber, char *message)
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

void DialCall(const char *phoneNumber)
{
    Serial.println("Calling...");
    gsmSerial.print("ATD");
    gsmSerial.print(phoneNumber);
    gsmSerial.println(";");
    delay(100);
}



void displayInfo()
{
    Serial.print(F("Location: "));
    if (gps.location.isValid())
    {
        Serial.print(gps.location.lat(), 6);
        Serial.print(F(","));
        Serial.print(gps.location.lng(), 6);
    }
    else
    {
        Serial.print(F("INVALID"));
    }
    Serial.println();
}
