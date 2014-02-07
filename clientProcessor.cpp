#include "clientProcessor.h"
#include "global.h"
#include <stdio.h>
#include <string.h>

int CClientProcessor::ProcessMessage(char* clientMessage, int read_size) {

    int responseType = 502; // command not implemented

    char *command;

    for (int i = 0; i < 4; i++) {
        command[i] = clientMessage[i];
    }
    command[4] = '\n';

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
    else if (type == 221){
        strcpy(serverMessage, "221 Service closing transmission channel\r\n");
        return -1;
    }
    else if (type == 250)
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

int CClientProcessor::Helo(char* clientMessage, int read_size) {

    return 250;
}

int CClientProcessor::Mail(char* clientMessage, int read_size) {

    return 502;
}

int CClientProcessor::Rcpt(char* clientMessage, int read_size) {

    return 502;
}

int CClientProcessor::Data(char* clientMessage, int read_size) {

    return 502;
}
