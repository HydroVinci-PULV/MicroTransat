// documentation pour le module Iridium https://docs.rockblock.rock7.com/docs/getting-started

#include <Common/IridiumSBD.h>

#define IridiumSerial Serial1 // le port série utilisé pour l'Iridium, à changer si besoin

#define DIAGNOSTICS true // pour activer ou désactiver les diagnostics du modem

#define DEBUG true // pour activer ou désactiver les messages de debug

IridiumSBD modem(IridiumSerial);

void printFirmwareRevision();

void printSignalQuality();

void setup()
{
    int err;

    // Pour le debug sur le port série console
    Serial.begin(115200);
    while (!Serial)
        ;

    // Initialisation de la connection série avec le modem Iridium
    IridiumSerial.begin(19200);

    Serial.println("Starting modem...");
    err = modem.begin();
    if (err != ISBD_SUCCESS)
    {
        Serial.print("Begin failed: error ");
        Serial.println(err);
        if (err == ISBD_NO_MODEM_DETECTED)
            Serial.println("No modem detected: check wiring.");
        return;
    }

    // Pour notre débug on affiche la version du modem
    printFirmwareRevision();
    // tjrs pour le débug, on va récup la qualité du signal satellite
    printSignalQuality();
    
}

// Fonction pour envoyer un message
void sendMessage(String message)
{
    int err;
    char buf[ISBD_MAX_MESSAGE_LENGTH];
    int len;

    // On envoie le message
    len = message.length();
    message.toCharArray(buf, len + 1);
    err = modem.sendSBDText(buff);
    if (err != ISBD_SUCCESS)
    {
        Serial.print("SendMessage failed: error ");
        Serial.println(err);
        if (err == ISBD_SENDRECEIVE_TIMEOUT)
            Serial.println("Try again with a better view of the sky.");
        return;
    }
}

#if DEBUG
void printFirmwareRevision()
{
    char version[12];
    int err;
    err = modem.getFirmwareVersion(version, sizeof(version));
    if (err != ISBD_SUCCESS)
    {
        Serial.print("FirmwareVersion failed: error ");
        Serial.println(err);
        return;
    }
    Serial.print("Firmware Version is ");
    Serial.print(version);
    Serial.println(".");
}

void printSignalQuality(){
    int signalQuality = -1;
    int err;
    err = modem.getSignalQuality(signalQuality);
    if (err != ISBD_SUCCESS)
    {
        Serial.print("SignalQuality failed: error ");
        Serial.println(err);
        return;
    }

    Serial.print("Signal quality");
    Serial.print(signalQuality);
    Serial.println("/5.");
}

#endif

#if DIAGNOSTICS
void ISBDConsoleCallback(IridiumSBD *device, char c)
{
    Serial.write(c);
}

void ISBDDiagsCallback(IridiumSBD *device, char c)
{
    Serial.write(c);
}
#endif