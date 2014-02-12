#include "clientProcessor.h"
#include "global.h"
#include <stdio.h>
#include <string.h>
#include <sstream>
#include <iostream>
#include <sys/stat.h>
#include <iterator>

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

    cout << "client message: " << clientMessage << endl;

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

    } else if (strcasecmp(command, "RSET") == 0) {

        responseType = Rset(clientMessage, read_size);
    } else if (strcasecmp(command, "INIT") == 0) {

        responseType = Init(clientMessage, read_size);
    } else if (strcasecmp(command, "QUIT") == 0) {

        responseType = 221;

    }

    for (unsigned int i = 0; i < MESSAGE_LENGTH; i++)
        clientMessage[i] = 0;

    return Response(responseType);
}

int CClientProcessor::Response(int type) {

    char serverMessage[RESPONSE_LENGTH];
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

    for (unsigned int i = 0; i < RESPONSE_LENGTH; i++)
        serverMessage[i] = 0;

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

int CClientProcessor::Init(char* clientMessage, int read_size) {
    return 220;
}

int CClientProcessor::Helo(char* clientMessage, int read_size) {

    cout << "Received HELO" << endl;

    state = STATE_HELO;

    rcptCount = 0;

    msgFileName = NewFileName(path);

    cout << "Received HELO end" << endl;

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

int CClientProcessor::Rset(char* clientMessage, int read_size) {

    cout << "Received RSET" << endl;

    state = STATE_HELO;
    rcptCount = 0;

    remove(msgFileName.c_str());
    msgFileName = "";

    return 220;
}

const std::string currentDateTime() {
    time_t now = time(0);
    struct tm tstruct;
    char buf[80];
    tstruct = *localtime(&now);
    strftime(buf, sizeof (buf), "%Y-%m-%d.%X", &tstruct);

    return buf;
}

int CClientProcessor::Data(char* clientMessage, int read_size) {

    cout << "Received DATA" << endl;

    if (state != STATE_DATA) {
        state = STATE_DATA;

        /* Headers */
        if (!msgFile.is_open())
            msgFile.open(msgFileName.c_str(), fstream::in | fstream::out | fstream::app | fstream::binary);

        msgFile << string("From:\r\n") + AddressFrom.GetAddress() + string("\r\n");
        msgFile << string("To:\r\n");
        string rcpts = "";
        for (unsigned int i = 0; i < AddressTo.size(); i++) {
            if (i == 0) {
                rcpts = AddressTo.at(0).GetAddress();
            } else {
                rcpts += string("; ") + AddressTo.at(i).GetAddress();
            }
        }
        msgFile << rcpts << string("\r\n") << endl;
        msgFile << string("Date:\r\n") + currentDateTime() + string("\r\n");
        msgFile << string("Subject: \r\n");

        cout << "sending 354" << endl;
        
        return 354;
    }

    cout << "Receiving data..." << endl;

    if (!msgFile.is_open())
        msgFile.open(msgFileName.c_str(), fstream::in | fstream::out | fstream::app | fstream::binary);

    string messageText = clientMessage;
    messageText.erase(0, 5);
    msgFile << string(messageText);

    if (string(clientMessage).find(".") != string::npos) //\r\n.\r\n
    {
        cout << "end of message found" << endl;
        state = STATE_HELO;

        for (int i = 0; i < rcptCount; i++) {
            cout << "Sending to " << AddressTo.at(i).GetUser() << "@" << AddressTo.at(i).GetDomain() << endl;
            string userFileName = NewFileName(string("./") + AddressTo.at(i).GetDomain() + string("/") + AddressTo.at(i).GetUser() + mbox);

            if (!msgFile.is_open())
                msgFile.open(msgFileName.c_str(), fstream::in | fstream::out | fstream::app | fstream::binary);
            ofstream userFile(userFileName.c_str(), fstream::trunc | fstream::binary);

            msgFile.seekg(0, ios::end);
            ifstream::pos_type size = msgFile.tellg();
            msgFile.seekg(0);

            char* buffer = new char[size];

            msgFile.read(buffer, size);
            userFile.write(buffer, size);

            delete[] buffer;
            userFile.close();
            msgFile.close();
        }

        remove(msgFileName.c_str());
        msgFileName = "";

        return 250;
    }
    /*
        if (!msgFile.is_open())
            msgFile.open(msgFileName.c_str(), fstream::in | fstream::out | fstream::app | fstream::binary);

        string messageText = clientMessage;
        messageText.erase(0, 5);
        msgFile << string(messageText); */

    return 250;
}

string CClientProcessor::NewFileName(string directory) {

    int i = 1;

    struct stat buf;
    string newFileName = directory + "1";

    if (stat(newFileName.c_str(), &buf) == -1) {
        return newFileName;
    } else {
        while (stat(newFileName.c_str(), &buf) > -1) {

            i++;

            stringstream ss;
            ss << i;

            newFileName = directory + ss.str();
        }


    }

    return newFileName;
}



