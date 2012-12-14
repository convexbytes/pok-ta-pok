
#ifndef __UDPSocket_H__
#define __UDPSocket_H__



#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstring>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <iostream>



class USock
{
    USock();

public:
    ~USock();

    static USock & instance();
    void Initial(const char *host, int port);

    int Receive(char *buffer) ;
    int Send(const char *buffer);

private:
    bool        mIsInitialOK;
    sockaddr_in mAddress;

    int mSockfd;


};

#endif



