/* 
 * File:   ClientProcessor.h
 * Author: oriona
 *
 * Created on 7 luty 2014, 21:16
 */

#ifndef CLIENTPROCESSOR_H
#define	CLIENTPROCESSOR_H

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include "Address.h"

using namespace std;

class CClientProcessor{
    
private:
    int clientSock;
    int Helo(char *clientMessage, int read_size);
    int Mail(char *clientMessage, int read_size);
    int Rcpt(char *clientMessage, int read_size);
    int Data(char *clientMessage, int read_size);
    int state;
    string msgFileName;
    fstream msgFile;
    int rcptCount;
    Address AddressFrom;
    vector<Address> AddressTo;
    string path;
    string mbox;
    
    
public:
    CClientProcessor(int clientS);
    int Response(int type);
    int ProcessMessage(char *clientMessage, int read_size);
    string NewFileName(string directory);
};

enum states{
    STATE_INIT = 0,
    STATE_HELO,
    STATE_DATA,
    STATE_DATA_END
};

#endif	/* CLIENTPROCESSOR_H */

