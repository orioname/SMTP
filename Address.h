/* 
 * File:   CAdress.h
 * Author: oriona
 *
 * Created on 10 luty 2014, 19:20
 */

#ifndef ADDRESS_H
#define	ADDRESS_H


#include <string>

using namespace std;

class Address{
    
private:
    
    string user;
    string domain;
    string address;
    
public:
   
    Address();
    string GetAddress(){return address;}
    string GetUser(){return user;}
    string GetDomain(){return domain;}
    bool SetAddress(string addr);
    static bool ValidateAddress(string addr) {return ((addr.length() > 2) && 
            (addr.find("@") != string::npos) &&
            addr.find("@") > 0);};
};

#endif	/* ADDRESS_H */

