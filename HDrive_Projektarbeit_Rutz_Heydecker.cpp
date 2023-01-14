
// **********************************************************************************************************************************************************************

// Allgemeine Information
// ----------------------------

// Datum        : 03.01.2023
// Datei        : HDrive_Projektarbeit
// HW       	: Henschel robotics Servermotor
// HW     	    : 17HS19-1684S-PG27
// Autor        : Björn Rutz & Jan Heydecker
// Version      : 1.0

// Version      : 2.0
// Aenderung    : Die Hilfsvariablen der ButtenState konnten Rausgenommen werden,
//              : jedoch wird nun ein Sleep von 1ms benötigt, damit nicht während dem Senden das Telegramm überschrieben wird.

// **********************************************************************************************************************************************************************


// Start Imports und definitionen
// **********************************************************************************************************************************************************************
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <string>
#include <windows.h>

using namespace std;

#pragma comment(lib,"ws2_32.lib")

// **********************************************************************************************************************************************************************
// Ende Imports und definitionen


// Start Main
// **********************************************************************************************************************************************************************
int main(void) {
    // Deffinition der Variablen und Objekte
    int PORT;                                                   // Server Port, der User muss diese einfügen
    string SERVERADDRESS;                                       // server adresse, der User muss diese einfügen
    const int MESSAGELENGTH = 512;                              // max Länger der Nachricht
    string stringTelegramm;                                     // Telegramm wird später im Code definiert
    
    SOCKET tcpSocket;                                           // Socket Objekt erstellen, benötigt für socket()
    struct sockaddr_in serverAdress, clientAddress;             // Structs in denen wir die Daten zum Server und Client Speichern
    int slen;                                                   // Variable gebraucht in sendto()
    char message[MESSAGELENGTH];                                // in message speichern wir die erhaltenen/versendeten Daten
    WSADATA wsaData;                                            // WSADATA Objekt, benötigt für WSAStartup()
    slen = sizeof(clientAddress);                               // Länge der Client Adresse, gebruacht in sendto()

    // Port und IP-Adresse einlesen 
    cout << "Bitte Motor IP eingeben" << endl;
    cin >> SERVERADDRESS;                                       
    cout << "Bitte Motor Port eingeben" << endl;
    cin >> PORT;                                                

    // Initialise Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        cout << "WSAStartup fehlgeschlagen: " << WSAGetLastError() << endl;
        system("pause");
        return 1;
    }

    // Erstelle den TCP Socket
    if ((tcpSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == INVALID_SOCKET) {
        cout << "Socket erstellen fehlgeschlagen: " << WSAGetLastError() << endl;
        system("pause");
        return 1;
    }

    // Reserviere den Platz im Memory für die Daten
    memset((char*)&serverAdress, 0, sizeof(serverAdress));

    // Setze Server Parameter
    serverAdress.sin_family = AF_INET;
    serverAdress.sin_port = htons(PORT);
    serverAdress.sin_addr.S_un.S_addr = inet_addr(SERVERADDRESS.c_str());

    // Verbinde mit dem TCP Server
    if (connect(tcpSocket, (struct sockaddr*)&serverAdress, slen) == INVALID_SOCKET) {
        cout << "Verbindung zu " << SERVERADDRESS.c_str() << " fehlgeschlagen: " << WSAGetLastError() << endl;
        system("pause");
        return 1;
    }

    // Diverse Ausgaben CMD
    cout << "Verbunden mit Server " << SERVERADDRESS.c_str() << endl;
    cout << "Bitte gewuenschte Taste Druecken: " << endl;
    cout << "W -> Drehung Rechts" << endl;
    cout << "S -> Drehung Links" << endl;
    cout << "A -> Programm beenden" << endl;
    
    // Gewährleistet das dauerhafte Senden und schliesst nicht nach Empfang einer Nachricht
    while (!(GetAsyncKeyState('A') & 0x8000)) {
        // Programmteil für den Rechtslauf
        if (GetAsyncKeyState('W') & 0x8000) {
            // Telegramm für den Rechtslauf
            stringTelegramm = "<control pos = \"0\" speed = \"5000\" torque = <\"8000\" mode = \"128\" acc = \"200000\" decc = \"1\" / >";
        }

        // Programmteil für den Linkslauf
        else if (GetAsyncKeyState('S') & 0x8000) {
            // Telegramm für den Linkslauf
            stringTelegramm = "<control pos = \"100000\" speed = \"5000\" torque = <\"-8000\" mode = \"128\" acc = \"200000\" decc = \"1\" / >";
        }

        // Programmteil für den Stopp
        else {
            // Telegramm für den Stopp
            stringTelegramm = "<control pos = \"0\" speed = \"5000\" torque = <\"8000\" mode = \"0\" acc = \"200000\" decc = \"1\" / >";
        }

        // Umwandlung des Telegramms von einem String zu einem Char
        char* charTelegramm = new char[stringTelegramm.length() + 1];
        for (int i = 0; i < stringTelegramm.length() + 1; i++) {
            charTelegramm[i] = stringTelegramm[i];
        }
 
        // Senden des Telegramms mit dem Datentyp Char
        if (send(tcpSocket, charTelegramm, strlen(message), 0) == SOCKET_ERROR) {
            cout << "Daten senden fehlgeschlagen: " << WSAGetLastError() << endl;
            return 1;
        }
        
        // Wird hier benötigt damit das Telegramm zuerst verschickt wird, bevor die Umwandlung des Telegramms wieder startet
        Sleep(1);
    }

    // Schliesse den TCP Socket
    closesocket(tcpSocket);
    WSACleanup();
    // Beende das Programm
    return 0;
}

// **********************************************************************************************************************************************************************
// Ende Main