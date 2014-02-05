/* 
 * File:   main.cpp
 * Author: oriona
 *
 * Created on 4 luty 2014, 21:55
 */
#include "global.h"
#include "server.h"

using namespace std;

int main(int argc, char* argv[])
{
  int sockd;
  struct sockaddr_in server;
  int status;

  /* create a socket */
  sockd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockd == -1)
  {
    perror("Socket creation error");
    exit(1);
  }

  struct hostent *host = gethostbyname("localhost");
  
  /* server address  */
  server.sin_family = AF_INET;
  server.sin_addr.s_addr = (*((in_addr*)(host->h_addr_list[0]))).s_addr; 
  server.sin_port = htons(SMTP_PORT); //2500 for now  

  status = bind(sockd, (struct sockaddr*)&server, sizeof(server));
  if (status == -1)
  {
    perror("Binding error");
    exit(1);
  }

  status = listen(sockd, 5);
  if (status == -1)
  {
    perror("Listening error");
    exit(1);
  }

  CServer cserver(sockd);
  cserver.acceptCon();
  
  return 0;
}

