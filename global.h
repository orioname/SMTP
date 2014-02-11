/* 
 * File:   global.h
 * Author: oriona
 *
 * Created on 4 luty 2014, 22:17
 */

#ifndef GLOBAL_H
#define	GLOBAL_H

#include <cstdlib>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define SMTP_PORT 2500
#define MAX_RCPT_COUNT 100
#define MESSAGE_LENGTH 2050
#define RESPONSE_LENGTH 100

#endif	/* GLOBAL_H */

