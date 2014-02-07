/* 
 * File:   ClientProcessor.h
 * Author: oriona
 *
 * Created on 7 luty 2014, 21:16
 */

#ifndef CLIENTPROCESSOR_H
#define	CLIENTPROCESSOR_H

class CClientProcessor{
    
private:
    int clientSock;
    int Helo(char *clientMessage, int read_size);
    int Mail(char *clientMessage, int read_size);
    int Rcpt(char *clientMessage, int read_size);
    int Data(char *clientMessage, int read_size);
    
public:
    CClientProcessor(int clientS) : clientSock(clientS) {};
    int Response(int type);
    int ProcessMessage(char *clientMessage, int read_size);
};

#endif	/* CLIENTPROCESSOR_H */

