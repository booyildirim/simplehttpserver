#include <stdio.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <regex.h>
#include <time.h>

char *decodeRequest(char *requestStr);
char *findUAgent(char *uAStr);
int isNumeric (const char * s);


int main( int argc, char *argv[] )
{
	if(argc > 2 || argc <2){
		printf("usage: myserver [portNumber]. port number should be higher than 1024. \n");
		return 1;
	}

    int portno = atoi(argv[1]);
    int sockfd, newsockfd, clilen;
    char buffer[2048];
    struct sockaddr_in serv_addr, cli_addr;
    int  n;

    /* First call to socket() function */
    sockfd = socket(AF_INET, SOCK_STREAM, 0); // TCP conn

    /* Initialize socket structure */
    // portno = 8080;	// argv[1]
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;	// accept from any ip
    serv_addr.sin_port = htons(portno);
 
    /* Now bind the host address using bind() call.*/
    bind(sockfd, (struct sockaddr *) &serv_addr,sizeof(serv_addr));
   
    /* Now start listening for the clients, here process will
    * go in sleep mode and will wait for the incoming connection
    */
    listen(sockfd,5);
    clilen = sizeof(cli_addr);

    while(1) { 

	    /* Accept actual connection from the client */
	    newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr,&clilen);

		int pid;// spawn a new process ! multithreadlike behaviour
		if ((pid = fork()) == -1)
        {
            close(newsockfd);
            continue;
        }
		else if(pid > 0)
        {
            close(newsockfd);
            continue;
        }
		else if(pid == 0)
        {
            
 			/* If connection is established then start communicating */
	    		n = read( newsockfd,buffer,2047 );
		
			char *msgToClnt = decodeRequest(buffer);
			//write(newsockfd,msgToClnt,sizeof(msgToClnt));
			send(newsockfd, msgToClnt, strlen(msgToClnt), 0);

        	close(newsockfd);
        	break;
        }   
		

    }
    close(sockfd);
    return 0; 
}

char *findUAgent(char *uAStr)
{
	if(strstr(uAStr,"Chrome") != NULL) {
		return "C";
	
	}
	else if(strstr(uAStr,"Firefox") != NULL) {
		return "F";
	}
	else if(strstr(uAStr,"MSIE") != NULL) {
		return "I";
	}
	else if(strstr(uAStr,"Safari") != NULL) {
		return "S";
	}
	else {
		return "U";
	}

}

int isNumeric (const char * s)
{
    if (s == NULL || *s == '\0' || isspace(*s))
      return 0;
    char * p;
    strtod (s, &p); // Convert string to double
    return *p == '\0';
}

char *decodeRequest(char *requestStr)
{
	char *buff = requestStr;

	char *getPart = strstr(buff,"GET /");	// if not get request, raise error !
	char *uAgentPart = strstr(buff,"User-Agent: ");	// if empty use Unknown as client !
	
	// byte size part
	char *byteInfoSlashed;
	byteInfoSlashed = strtok(getPart, " ");
	byteInfoSlashed = strtok(NULL,"/");
	char *byteNum = strtok(byteInfoSlashed," ");
	int byteSize = atoi (byteNum); // ex. byteSize = 100 as integer here ! any size between 100 and 20,000 bytes. If not raise error

	
	/* C, F, I or S chrome firefox ie safari 
	   otherwise U if different or unidentified*/
	char *uAgnt = findUAgent(uAgentPart);
	int i;
	// send uAgnt and byteSize to a function which returns the html document as a string
	char *htmlDoc;
	if( (byteSize < 100 || byteSize > 20000) || (isNumeric(byteNum) == 0) ) { // if the first token in request is numeric accept it too!
	
		// send bad request message
		strcpy(htmlDoc,"HTTP/1.1 400 Bad Request\r\n\r\n");
		strcat(htmlDoc,"Bad Request\r\n");
		time_t mytime;
   		mytime = time(NULL);
		// printf("bad request encountered %s\n", ctime(&mytime));
		printf("%s\n", htmlDoc);
	}
	else {
		time_t mytime;
   		mytime = time(NULL);
		printf("Transferring %d bytes. %s\n",byteSize,ctime(&mytime));

		strcpy(htmlDoc,"HTTP/1.1 200 OK\r\n");
		strcat(htmlDoc,"Content-Type: text/html\r\n");
		strcat(htmlDoc,"Content-Length: ");
		strcat(htmlDoc, byteNum);
		strcat(htmlDoc, "\r\n\r\n");
		strcat(htmlDoc, "<HTML>\n<HEAD>\n<TITLE>This document is ");
		strcat(htmlDoc, byteNum);
		strcat(htmlDoc, " bytes long</TITLE>\n</HEAD>\n<BODY>");
		
		// print the document here
		
		
		for(i=1;i<byteSize;i++)
		{
			if(i%2==0)
			{
				strcat(htmlDoc, uAgnt);
				
			}else{
				strcat(htmlDoc, " ");
				
				
			}

		}
		strcat(htmlDoc, "</BODY></HTML>");
		
		printf("%s\n", htmlDoc);
	    	

	}

	
	

	return htmlDoc;

}
