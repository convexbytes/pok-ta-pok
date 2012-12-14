#include "udpSocket.h"
#include "stdio.h"

USock::USock()
{
	mIsInitialOK = false;
}


USock::~USock()
{
}


USock & USock::instance()
{
    static USock u_sock;
    return u_sock;
}

void USock::Initial(const char *host, int port)
{
	struct hostent *host_ent ;
	struct in_addr *addr_ptr ;
    
    host_ent = (struct hostent *)gethostbyname(host);
	if ( (host_ent == 0) )
	{
		if (inet_addr(host) == INADDR_NONE)
			printf("Invalid host name \n");
	}
	else
	{
		addr_ptr = (struct in_addr *) *host_ent->h_addr_list ;
		host = inet_ntoa(*addr_ptr) ;
	}
    
	mSockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if( (mSockfd) < 0)
	{
		printf("Can't create socket \n") ;
	}

	memset(&mAddress, 0, sizeof(mAddress)) ;
	mAddress.sin_family         = AF_INET ;
	mAddress.sin_addr.s_addr    = htonl(INADDR_ANY) ;
	mAddress.sin_port           = htons(0) ;

	if(bind(mSockfd, (sockaddr *)&mAddress, sizeof(mAddress)) < 0)
	{
		close(mSockfd);

		printf("Can't bind client to any port \n");
	}

	memset(&mAddress, 0, sizeof(mAddress)) ;
	mAddress.sin_family         = AF_INET ;
	mAddress.sin_addr.s_addr    = inet_addr( host ) ;
	mAddress.sin_port           = htons( port ) ;
	mIsInitialOK                = true;
}



int USock::Receive(char *buffer)
{
	socklen_t servlen ;


	sockaddr_in serv_addr;
    servlen = sizeof(serv_addr);
    int n = recvfrom(mSockfd, buffer, 4096, 0, (sockaddr *)&serv_addr, &servlen);
	if (n > 0)
	{
		
		mAddress.sin_port = serv_addr.sin_port ;
	}

	return n ;
}

int USock::Send(const char *buffer)
{
	if (mIsInitialOK == true)
	{
		int n = std::strlen(buffer) ;
        n = sendto(mSockfd, buffer, 4096, 0, (sockaddr *)&mAddress, sizeof(mAddress));
		return n;
	}

	std::cout << buffer << std::endl;
	return std::strlen(buffer);
}
