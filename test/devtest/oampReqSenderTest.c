

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#define SERVERPORT "3790"

#include "interfaceAPI.h"




unsigned char pucListOfOIDs[][128] = 
{
"attemptedUDRDTR", 
"successfulUDADTR",
"failedUDADTR",
"attemptedPURDTU", 
"successfulPUADTU",
"failedPUADTU",
"suicide"
}
;
     
/*

unsigned char pucListOfOIDs[][128] = 
{



"imsSubscriptionDBU",
"privateUserIdDBU",
"sipPublicUserIdDBU",
"telPublicUserIdDBU",
"privateSrvIdDBU",
"sipPublicSrvIdDBU",
"telPublicSrvIdDBU",
"regPublicUserIdRoamingDomainDBU",
"unRegPublicUserIdDBU",
"regPrivateUserIdDBU",
"attemptedUARUR",  
"successfulUAAUR", 
"failedUAAUR",     
"attemptedSARUR",  
"successfulSAAUR",
"failedSAAUR",     
"attemptedLIALIQ", 
"successfulLIALIQ",        
"failedLIALIQ",    
"attemptedMARMA",
"successfulMAAMA", 
"failedMAAMA",     
"attemptedUDRDTR", 
"successfulUDADTR",
"failedUDADTR",
"attemptedPURDTU", 
"successfulPUADTU",
"failedPUADTU",
"listenPortBindFailed",    
"downDB",          
"failedThresholdUAR",
"failedThresholdSAR",
"failedThresholdMAR",
"failedThresholdLIR",
"failedThresholdUDR",
"failedThresholdPUR"
}
;*/


int main(int argc, char *argv[])
{
	int sockfd;
	struct addrinfo hints, *servinfo, *p;
	int rv;
	int numbytes;

	if (argc != 3) {
		fprintf(stderr,"usage: oampReqSenderTest hostname port\n");
		exit(1);
	}

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_DGRAM;

	if ((rv = getaddrinfo(argv[1], argv[2], &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
	}

	// loop through all the results and make a socket
	for(p = servinfo; p != NULL; p = p->ai_next) {
		if ((sockfd = socket(p->ai_family, p->ai_socktype,
				p->ai_protocol)) == -1) {
			perror("oampReqSenderTest: socket");
			continue;
		}

		break;
	}

	if (p == NULL) {
		fprintf(stderr, "oampReqSenderTest: failed to bind socket\n");
		return 2;
	}

	unsigned int unIndex=0;
	unsigned int	unSizeOfArray = sizeof(pucListOfOIDs)/128;
	xmlChar * poXmlBuff = NULL;

	printf("\n\n unSizeOfArray == %d \n\n",unSizeOfArray);

	for (unIndex=0 ; unIndex <unSizeOfArray ; unIndex++) 
	{
		if (0 == strncmp("suicide",pucListOfOIDs[unIndex] , 7)) 
		{
				if ((numbytes = sendto(sockfd, pucListOfOIDs[unIndex], strlen(pucListOfOIDs[unIndex]), 0,
				 p->ai_addr, p->ai_addrlen)) == -1) 
				{
					perror("oampReqSenderTest: sendto");
					exit(1);
				}
		}
		else
		{
		
			printf("\n\n About to send OID => %s ",pucListOfOIDs[unIndex]);
	
			poXmlBuff = oampMakeRequest (pucListOfOIDs[unIndex], "GET", 0);
	
			if ((numbytes = sendto(sockfd, poXmlBuff, strlen(poXmlBuff), 0,
				 p->ai_addr, p->ai_addrlen)) == -1) {
			perror("oampReqSenderTest: sendto");
			exit(1);
		}
		}

		
		printf("oampReqSenderTest: sent %d bytes to %s\n", numbytes, argv[1]);
	}
	

	freeaddrinfo(servinfo);

	
	close(sockfd);

	return 0;
}
