
#include "server.h"
#include "clientProcessor.h"
#include <pthread.h>
#include<string.h>    
#include<stdlib.h> 



void *processClient(void *sockdClient)
{
    int sock = *(int*)sockdClient;
    char client_message[2050];
    int read_size;
    CClientProcessor cp(sock);
     
    while((read_size = recv(sock , client_message , sizeof(client_message) , 0)))
    {
        	if(cp.ProcessMessage(client_message, read_size) == -1)
		{
			printf("Connection thread closing\n");
                        free(sockdClient);
			exit(0);
		}
    }
         
    free(sockdClient);
    exit(-1);
}

int CServer::acceptCon(){
    
    struct sockaddr_in peer_name;
    int sockdClient, *sockClientNew;
    socklen_t addrlen = sizeof(peer_name);
    
    /* wait for a connection */
    while ((sockdClient = accept(sockd, (struct sockaddr*)&peer_name, &addrlen))){
    
        pthread_t threadID;
        sockClientNew = (int *) malloc(1);
        *sockClientNew = sockdClient;
         
        if( pthread_create( &threadID , NULL ,  processClient , (void*) sockClientNew) < 0)
        {
            perror("Could not create thread");
            return 1;
        }
         
        pthread_join( threadID , NULL);
          
    }
    
    if (sockdClient == -1)
    {
      perror("Wrong connection");
      exit(1);
    }
    
    return 0;
}


