/* 
 * File:   server.h
 * Author: oriona
 *
 * Created on 4 luty 2014, 23:10
 */

#ifndef SERVER_H
#define	SERVER_H

#include "global.h"

class CServer{
private:
    int sockd;
public:
    CServer (int ssockd) : sockd(ssockd) {};
    int acceptCon ();
};

#endif	/* SERVER_H */

