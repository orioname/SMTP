#include "clientProcessor.h"
#include "global.h"
#include <stdio.h>
#include <string.h>
#include <sstream>
#include <iostream>
#include <sys/stat.h>

using namespace std;

CClientProcessor::CClientProcessor(int clientS) {
    clientSock = clientS;
    state = STATE_INIT;
    rcptCount = 0;
    path = "./";
    mbox = "/mbox/";
}

int CClientProcessor::ProcessMessage(char* clientMessage, int read_size) {

    int responseType = 502; // command not implemented

    char command[5] = "comm";

    if (read_size >= 4)
        strncpy(command, clientMessage, 4);

    if (strcasecmp(command, "HELO") == 0) {

        responseType = Helo(clientMessage, read_size);

    } else if (strcasecmp(command, "EHLO") == 0) {

        responseType = Helo(clientMessage, read_size);

    } else if (strcasecmp(command, "MAIL") == 0) {

        responseType = Mail(clientMessage, read_size);

    } else if (strcasecmp(command, "RCPT") == 0) {

        responseType = Rcpt(clientMessage, read_size);

    } else if (strcasecmp(command, "DATA") == 0) {

        responseType = Data(clientMessage, read_size);

    } else if (strcasecmp(command, "QUIT") == 0) {

        responseType = 221;

    }

    return Response(responseType);
}

int CClientProcessor::Response(int type) {

    char serverMessage[100];
    int write_size;

    if (type == 220)
        sprintf(serverMessage, "220 Welcome to electronic mail system \r\n");
    else if (type == 221) {
        strcpy(serverMessage, "221 Service closing transmission channel\r\n");
        return -1;
    } else if (type == 250)
        strcpy(serverMessage, "250 OK\r\n");
    else if (type == 354)
        strcpy(serverMessage, "354 Start mail input; end with <CRLF>.<CRLF>\r\n");
    else if (type == 501)
        strcpy(serverMessage, "501 Syntax error in parameters or arguments\r\n");
    else if (type == 502)
        strcpy(serverMessage, "502 Command not implemented\r\n");
    else if (type == 503)
        strcpy(serverMessage, "503 Bad sequence of commands\r\n");
    else if (type == 550)
        strcpy(serverMessage, "550 No such user\r\n");
    else if (type == 551)
        strcpy(serverMessage, "551 User not local. Can not forward the mail\r\n");
    else
        sprintf(serverMessage, "%d No description\r\n", type);

    write_size = (int) strlen(serverMessage);

    write(clientSock, serverMessage, write_size);

    return 0;
}

string extractAddress(string message) {
    string addr = "";

    size_t pos1 = message.find("<");
    size_t pos2 = message.find(">");

    addr = message.substr(0, pos2);
    addr.erase(0, pos1 + 1);

    return addr;
}

string extractUser(string addr) {
    string usr = "";

    size_t pos = addr.find("@");

    usr = addr.substr(0, pos);

    return usr;
}

string extractDomain(string addr) {
    string dom = "";

    size_t pos = addr.find("@");

    dom = addr.erase(0, pos + 1);

    return dom;
}

int CClientProcessor::Helo(char* clientMessage, int read_size) {

    cout << "Received HELO" << endl;

    state = STATE_HELO;

    rcptCount = 0;

    msgFileName = NewFileName(path);

    return 250;
}

int CClientProcessor::Mail(char* clientMessage, int read_size) {

    cout << "Received MAIL" << endl;

    if (state != STATE_HELO)
        return Response(503);

    string fromAddress;
    fromAddress = extractAddress(string(clientMessage));

    if (!Address::ValidateAddress(fromAddress))
        return 501;

    cout << "Sender: " << fromAddress << endl;

    AddressFrom.SetAddress(fromAddress);

    return 250;
}

int CClientProcessor::Rcpt(char* clientMessage, int read_size) {

    cout << "Received RCPT" << endl;

    if (state != STATE_HELO)
        return Response(503);

    if (rcptCount > MAX_RCPT_COUNT)
        return Response(552);

    string toAddress;
    string toDomain;
    string toUser;
    toAddress = extractAddress(string(clientMessage));

    cout << toAddress << endl;

    if (!Address::ValidateAddress(toAddress))
        return 501;

    toUser = extractUser(toAddress);
    toDomain = extractDomain(toAddress);

    cout << "Recipient: " << toUser << "@" << toDomain << endl;

    struct stat sb;

    if (!(stat((path + toDomain).c_str(), &sb) == 0 && S_ISDIR(sb.st_mode))) {
        cout << "Recipient not local" << endl;
        return 551;
    }

    if (!(stat((path + toDomain + "/" + toUser).c_str(), &sb) == 0 && S_ISDIR(sb.st_mode))) {
        cout << "Recipient not found on this domain" << endl;
        return 550;
    }

    AddressTo.push_back(Address());
    AddressTo.at(rcptCount).SetAddress(toAddress);
    rcptCount++;

    return 250;
}

int CClientProcessor::Data(char* clientMessage, int read_size) {

    cout << "Received DATA" << endl;

    if (state != STATE_DATA) {
        state = STATE_DATA;
        return 354;
    }

    cout << "Receiving data..." << endl;

    if (string(clientMessage).find("koniec") != string::npos) //\r\n.\r\n
    {
        state = STATE_HELO;
        msgFile.close();
        for (int i = 0; i < rcptCount; i++) {
            cout << "Sending to " << AddressTo.at(i).GetUser() << "@" << AddressTo.at(i).GetDomain() << endl;
            //TODO copy to user folder
        }

        //remove(msgFileName.c_str());
        //msgFileName = "";

        return 250;
    }

    if (!msgFile.is_open())
        msgFile.open(msgFileName.c_str(), fstream::in | fstream::out | fstream::app);

    string messageText = clientMessage;
    messageText.erase(0,4);
    msgFile << string(messageText);

    return 220;
}

string CClientProcessor::NewFileName(string directory) {

    int i = 0;

    struct stat buf;
    string newFileName = directory + "msg0.txt";

    if (stat(newFileName.c_str(), &buf) == -1) {
        return newFileName;
    } else {
        while (stat(newFileName.c_str(), &buf) > -1) {

            i++;

            stringstream ss;
            ss << i;

            newFileName = string("msg") + ss.str() + ".txt";
        }


    }

    return newFileName;
}

