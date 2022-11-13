/* 
 * server FTP program
 *
 * NOTE: Starting homework #2, add more comments here describing the overall function
 * performed by server ftp program
 * This includes, the list of ftp commands processed by server ftp.
 *
 */
/* HOME WORK 2
*	Vito and Jason FTP project 
*
*	commands implemented by Vito
*	user
*	pass
*	mkdir
*	rmdir
*	dele
*	ls
*	pwd
*	cd
*
*	commands implemented by Jason
*	recv
*	put
*	stat
*	quit
*	help
*
*	
*	We tested the code together 
*/
#include <dirent.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>

#define SERVER_FTP_PORT 2024
#define DATA_CONNECTION_PORT 2025


/* Error and OK codes */
#define OK 0
#define ER_INVALID_HOST_NAME -1
#define ER_CREATE_SOCKET_FAILED -2
#define ER_BIND_FAILED -3
#define ER_CONNECT_FAILED -4
#define ER_SEND_FAILED -5
#define ER_RECEIVE_FAILED -6
#define ER_ACCEPT_FAILED -7


/* Function prototypes */

int svcInitServer(int *s);
int sendMessage (int s, char *msg, int  msgSize);
int receiveMessage(int s, char *buffer, int  bufferSize, int *msgSize);


/* List of all global variables */

char userCmd[1024];	/* user typed ftp command line received from client */
char cmd[1024];		/* ftp command (without argument) extracted from userCmd */
char argument[1024];	/* argument (without ftp command) extracted from userCmd */
char replyMsg[1024];       /* buffer to send reply message to client */

char * pCwd;
char cwd[1024];/* this is to store the current working dir*/
int status;

char * users[] = {"Vito", "Anthony"};
char * passwords[] = {"Vito", "Anthony"};
char * currentUser;
int passwordIndex;
int readyToLogin;
int logged_in;
int not_quit = 1;

/*
 * main
 *
 * Function to listen for connection request from client
 * Receive ftp command one at a time from client
 * Process received command
 * Send a reply message to the client after processing the command with staus of
 * performing (completing) the command
 * On receiving QUIT ftp command, send reply to client and then close all sockets
 *
 * Parameters
 * argc		- Count of number of arguments passed to main (input)
 * argv  	- Array of pointer to input parameters to main (input)
 *		   It is not required to pass any parameter to main
 *		   Can use it if needed.
 *
 * Return status
 *	0			- Successful execution until QUIT command from client 
 *	ER_ACCEPT_FAILED	- Accepting client connection request failed
 *	N			- Failed stauts, value of N depends on the command processed
 */

int main(	
	int argc,
	char *argv[]
	)
{
	/* List of local varibale */

	int msgSize;        /* Size of msg received in octets (bytes) */
	int listenSocket;   /* listening server ftp socket for client connect request */
	int ccSocket;        /* Control connection socket - to be used in all client communication */
	int dcSocket;
	int status;
	int cmdStatus;      // return value from commands

	//int bytesReceived;


	/*
	 * NOTE: without \n at the end of format string in printf,
         * UNIX will buffer (not flush)
	 * output to display and you will not see it on monitor.
	*/
	printf("Started execution of server ftp\n");


	 /*initialize server ftp*/
	printf("Initialize ftp server\n");	/* changed text */

	status = svcInitServer(&listenSocket);
	if(status != 0)
	{
		printf("Exiting server ftp due to svcInitServer returned error\n");
		exit(status);
	}


	printf("ftp server is waiting to accept connection\n");

	/* wait until connection request comes from client ftp */
	ccSocket = accept(listenSocket, NULL, NULL);

	printf("Came out of accept() function \n");

	if(ccSocket < 0)
	{
		perror("cannot accept connection:");
		printf("Server ftp is terminating after closing listen socket.\n");
		close(listenSocket);  /* close listen socket */
		return (ER_ACCEPT_FAILED);  // error exist
	}

	printf("Connected to client, calling receiveMsg to get ftp cmd from client \n");


	/* Receive and process ftp commands from client until quit command.
 	 * On receiving quit command, send reply to client and 
         * then close the control connection socket "ccSocket". 
	 */
	do
	{
	    /* Receive client ftp commands until */
 	    status=receiveMessage(ccSocket, userCmd, sizeof(userCmd), &msgSize);
 	    userCmd[msgSize] = '\0';
	    if(status < 0)
	    {
		printf("Receive message failed. Closing control connection \n");
		printf("Server ftp is terminating.\n");
		break;
	    }


/*
 * Starting Homework#2 program to process all ftp commandsmust be added here.
 * See Homework#2 for list of ftp commands to implement.
 */
	    /* Separate command and argument from userCmd */
	    //strcpy(cmd, userCmd);  /* Modify in Homework 2.  Use strtok function */
	    //strcpy(argument, "");  /* Modify in Homework 2.  Use strtok function */
	    /****************            Important info			  *********************/
	    /*
	    * If the users is not logged in then the only commands that will work are 
	    * user
	    * stat
	    * help
	    * pass
	    * quit 
	    *
	    * each command is compared with strcmp if it returns 0 then it is true becasue 0 == 0
	    * (argument != '\0') that checks if there is an argument
	    * (logged_in) this checks if the user is logged_in
	    */
	    /**************** spliting the command and argument  **********************/
	    char * cmd;
		char * argument;

		cmd = strtok(userCmd, " ");

		//printf("the commanded enter is : %s\n", userCmd);

		argument = strtok(NULL, " ");

		if(cmd == NULL){
			printf("No ftp cmd was received\n");
		} else {
			printf("ftp cmd names is : %s \n", cmd);
		}
		
		//argument = strtok(userCmd, " ");

		if(argument != NULL){
			printf("argument is : %s \n", argument);
		}else {
			printf("no argument\n");
		}
		/**************** Reset reply  **********************/
		strcpy(replyMsg, "502 Command not implemented.");

		/**************** user **********************/

			if (strcmp(cmd, "user") == 0){
				//used to check if the argument is empty
				if (argument != '\0'){
				
	
    				int userNotFound = 1;
					int  i = 0;
					for (; i < sizeof(users) / sizeof(users[0]); ++i){
						if(strcmp(argument, users[i]) == 0){
							//printf("%s\n", users[i]); used to debug
							//currentUser = users[i];
							passwordIndex = i;
							readyToLogin = 1;
							userNotFound = 0;
							strcpy(replyMsg, "331 User name okay, need password.\n");
							//strcat(replyMsg, "User:");
							//strcat(replyMsg, users[i]);
							//strcat(replyMsg, "331 User name okay, need password.\n");
							break;
							}
					}//end for loop
					if(userNotFound)
					{
						strcpy(replyMsg, "332 that user doesn't exist\n");
					}
				} else {
					strcpy(replyMsg, "501 Syntax error in parameters or arguments.\n");
				}
			}//end of user command

			/**************** Pass **********************/

			if(strcmp(cmd, "pass") == 0){
				if (argument != '\0')
				{
					if (readyToLogin)
					{
						
					
				

						//checks the password that is connected with the user
						if(strcmp(argument, passwords[passwordIndex]) == 0){
							currentUser = users[passwordIndex];
							logged_in = 1;
							strcpy(replyMsg, "332 Password successful you are now loged in.");
							//strcat(replyMsg, currentUser);
							//strcat(replyMsg, "\n");
						} //end of password check
						else 
						{
							strcpy(replyMsg, "530 Invalid password.\n");
						}
					} 
					else 
					{
						strcpy(replyMsg, "332 Need account for login.\n");
					}
			} 
			else 
			{
				strcpy(replyMsg, "501 Syntax error in parameters or arguments.\n");
			}
		}//end of pass command

		/**************** Help **********************/

		if (strcmp(cmd, "help") == 0)
        {
            strcpy(replyMsg, "211\nCommands\t Syntax\t\t Function\n"

                    "user  \t user [Username]    \t Lets user use pass cmd\n"
                    "pass  \t pass [Password]    \t Lets user login\n"
                    "mkdir \t mkdir [Dir_Name]   \t Makes a new folder\n"

                    "rmdir \t rmdir [Dir_Name]   \t Removes a folder\n"
                    "cd    \t cd [Dir_Name]      \t Changes current working directory\n"
                    "dele  \t dele [file_Name]   \t Removes a file\n"
                    "pwd   \t pwd                \t Prints current working directory\n"
                    "ls    \t ls                 \t List all files in current directory\n"
                    "stat  \t stat               \t Prints status\n"
                    "quit  \t quit               \t quits program\n"
                    "put   \t put [file_Name]    \t puts file on server from client\n"
                    "recv  \t recv [file_Name]   \t receive file from server\n"
            );

        }//end of help command

		/**************** stat **********************/

		if (strcmp(cmd, "stat") == 0)
            {
                strcpy(replyMsg, "200 Command is Okay. Transfer mode is ASCII.\n");
            }//end of stat command

		/**************** pwd **********************/

		if (strcmp(cmd , "pwd") == 0)
		{
			if(logged_in)
			{
				//gets current working dir
				getcwd(cwd, sizeof(cwd));
				strcpy(replyMsg, "Current working dir: ");
    			strcat(replyMsg, cwd);
    		}
    		else
    		{
    			strcpy(replyMsg, "530 not logged in.\n");
    		}
		} //end of pwd command

		/**************** ls **********************/
		if (strcmp(cmd , "ls") == 0)
		{
			if(logged_in)
			{
				DIR *d;//Dir pointer

    			struct dirent *dir;

    			d = opendir(".");//calls open dir function
    			if (d)
    			{
    				strcpy(replyMsg, "ls:\n");
    	    		while ((dir = readdir(d)) != NULL)
    	    		{
           		 		printf("%s\n", dir->d_name);
           		 		strcat(replyMsg, dir->d_name);
           		 		strcat(replyMsg, " \n");
        			}
        			closedir(d);
   		 		}
   		 	}
   		 	else
   		 	{
   		 		strcpy(replyMsg, "530 not logged in.\n");
   		 	}
   		} //end of ls command
		

		/**************** mkdir **********************/

		if (strcmp(cmd, "mkdir") == 0)
		{
			if (argument == '\0')
			{
				strcpy(replyMsg, "501 Syntax error in parameters or arguments.\n");
			}
			else
			{
				if(logged_in)
				{
					//checks if the command worked if return value is not -1 then it worked
					cmdStatus = mkdir(argument, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
					if (cmdStatus != -1)
					{
						strcpy(replyMsg, "212 successfully created dir /");
						strcat(replyMsg, argument);
						strcat(replyMsg, "\n");
					} 
					else
					{
						printf("212 function failed dir was not made\n");
					}
				}//end of logged in if
				else
				{
					strcpy(replyMsg, "530 not logged in.\n");
				}
			}//end of else
		}//end of mkdir

		/**************** rmkdir **********************/

		if (strcmp(cmd, "rmdir") == 0)
		{
			if (argument == '\0')
			{
				strcpy(replyMsg, "501 Syntax error missing parameters\n");

			}//end of check argument if
			else 
			{
				if(logged_in)
				{
					//checks if the command worked if return value is 0 then it worked
			  		cmdStatus = rmdir(argument);
			  		if (cmdStatus == 0)
			  		{
			  			strcpy(replyMsg, "212 successfully removed dir ");
			  			strcat(replyMsg, argument);
			  		} 
			  		else 
			  		{
			  			strcpy(replyMsg, "212 Unable to remove the directory.\n");
			  		}
			  	}//end of check user if
			  	else
			  	{
			  		strcpy(replyMsg, "530 not logged in.\n");
			  	}
			}//end of arg else

		}//end of rmdir

		/**************** dele **********************/

		if (strcmp(cmd, "dele") == 0)
		{
			if (argument == '\0')
			{
				strcpy(replyMsg, "501 Syntax error missing parameters\n");
			}//end arg if
			else 
			{
				if(logged_in)
				{
					//checks if the command work if it returns a 0 then it worked
					cmdStatus = unlink(argument);
					if(cmdStatus == 0)
					{
						strcpy(replyMsg, "213 Command okay. File: ");
						strcat(replyMsg, argument);
						strcat(replyMsg, " was successfully removed.");
					} 
					else 
					{
						strcpy(replyMsg, "213 file was unable to be removed.\n");
					}
				}//end of loged in if
				else
				{
					strcpy(replyMsg, "530 not logged in.\n");
				}
			}//end of arg else
		}//end of dele command

		/**************** cd **********************/

		if (strcmp(cmd , "cd") == 0)
		{
			//Checks if theres is no argument 
			if(argument == '\0')
			{
				strcpy(replyMsg, "501 Syntax error in parameters or arguments.\n");
			} //end of arg if
			else 
			{
				if(logged_in)
				{
				
					cmdStatus = chdir(argument);
					if (cmdStatus == 0)
					{
						strcpy(replyMsg, "212 Current dir has been changed");
						pCwd = getcwd(cwd, sizeof(cwd));
						if (pCwd != NULL)
						{
							strcat(replyMsg, " to ");
							strcat(replyMsg, pCwd);
						} 
					} 
					else 
					{
						strcpy(replyMsg, argument);
						strcat(replyMsg, " Does not exist");
					}
				}//end of check logged_in
				else
				{
					strcpy(replyMsg, "530 not logged in.\n");//prints if user is not loggged in
				}//end of log else
			}//end of arg else

		}//end of cd command

		/**************** recv **********************/
		if (strcmp(cmd, "recv") == 0)
		{
			if(argument == '\0')
			{
				strcpy(replyMsg, "501 Syntax error in parameters or arguments.\n");
			} //end of arg if
			else
			{
				if(logged_in)
				{
					//printf("recv Command has been excuted\n");
					status = dcclntConnect("10.3.200.17", &dcSocket); //accept dc connection
						if(status != 0)
						{
							printf("Exiting server ftp due to svcInitServer returned error\n");
							exit(status);
						}
						printf("connecnnected too client\n");
					FILE *fptr = fopen(argument, "r"); //opens local file on server
					if(fptr == NULL)
					{
						printf("213 file could not be open\n");
						strcpy(replyMsg, "213 file could not be open\n");
					}
					else//if the file does not return null this code is excuted
					{
						printf("File open\n");
						while(!feof(fptr))
						{
							char temp[100];
							int bytesRead = fread(temp, sizeof(char), 100, fptr);
							status = sendMessage(dcSocket, temp, bytesRead);
							if(status != OK || bytesRead < 0)
								{
		   							 break;
								}
						}
						strcpy(replyMsg, "");//sets message to nothing
				}//end of file check else
				printf("closing file\n");
				fclose(fptr); //close file
				printf("closing socket\n");
				close(dcSocket); //close data connection
				
				}//end of login check
				else
				{
					strcpy(replyMsg, "530 not logged in.\n");//prints if user is not loggged in
				}

			}//arg else

		}//end of recv command








		/**************** put **********************/
		int bytesReceived;
		if(strcmp(cmd, "put") == 0)
		{
			if(argument == '\0')
			{
				strcpy(replyMsg, "501 Syntax error in parameters or arguments.\n");
			} //end of arg if
			else
			{
				if(logged_in)
				{
					//printf("Send Command excuted\n");
					status = dcclntConnect("10.3.200.17", &dcSocket); //accept dc connection
					//checks if status failled
					if(status != 0)
					{
						printf("213 File was not able to be open.\n");
					}
					else
					{
						printf("connecnnected too client\n");

						FILE *fptr = fopen(argument, "w"); //open local file in write mode
						//checks if fopen failed code excutes if it does not fail
						if(fptr == NULL)
						{
							printf("213 file could not be open\n");
							strcpy(replyMsg, "213 file could not be open\n");
						}
						else
						{
							printf("File Open\n");
							do
							{
								//printf("in the while loop\n");
								char temp[100];
								bytesReceived = -1;
								status = receiveMessage(dcSocket, temp, sizeof(temp), &bytesReceived);
								if(status != OK)
								{
									break;
								}
								int bytesWritten = fwrite(temp, sizeof(char), bytesReceived, fptr); //read from loacl file
							}
							while(bytesReceived > 0);
						}
						fclose(fptr);
					}
				
					printf("closing file\n");
					close(dcSocket);
					printf("closing socket\n");
					strcpy(replyMsg, "");//sets message to nothing
				}//end of login check
				else
				{
					strcpy(replyMsg, "530 not logged in.\n");//prints if user is not loggged in
				}
			}//end of arg else
		}//end of put cmd
		

		close(dcSocket);
		//printf("closing socket\n");



		/**************** quit **********************/
		if(strcmp(cmd, "quit") == 0){
			strcpy(replyMsg, "cmd 231 okay, user logged out\n");
			not_quit = 0;
		}
		/**************** End of Commands **********************/
	    /*
 	     * ftp server sends only one reply message to the client for 
	     * each command received in this implementation.
	     */

	    //strcpy(replyMsg,"200 cmd okay\n");  /* Should have appropriate reply msg starting HW2 */
	    status=sendMessage(ccSocket,replyMsg,strlen(replyMsg) + 1);	/* Added 1 to include NULL character in */
				/* the reply string strlen does not count NULL character */
	    if(status < 0)
	    {
		break;  /* exit while loop */
	    }
	}
	while(not_quit);

	printf("Closing control connection socket.\n");
	close (ccSocket);  /* Close client control connection socket */

	printf("Closing listen socket.\n");
	close(listenSocket);  /*close listen socket */

	printf("Existing from server ftp main \n");

	return (status);
}


/*
 * svcInitServer
 *
 * Function to create a socket and to listen for connection request from client
 *    using the created listen socket.
 *
 * Parameters
 * s		- Socket to listen for connection request (output)
 *
 * Return status
 *	OK			- Successfully created listen socket and listening
 *	ER_CREATE_SOCKET_FAILED	- socket creation failed
 */

int svcInitServer (
	int *s 		/*Listen socket number returned from this function */
	)
{
	int sock;
	struct sockaddr_in svcAddr;
	int qlen;

	/*create a socket endpoint */
	if( (sock=socket(AF_INET, SOCK_STREAM,0)) <0)
	{
		perror("cannot create socket");
		return(ER_CREATE_SOCKET_FAILED);
	}

	/*initialize memory of svcAddr structure to zero. */
	memset((char *)&svcAddr,0, sizeof(svcAddr));

	/* initialize svcAddr to have server IP address and server listen port#. */
	svcAddr.sin_family = AF_INET;
	svcAddr.sin_addr.s_addr=htonl(INADDR_ANY);  /* server IP address */
	svcAddr.sin_port=htons(SERVER_FTP_PORT);    /* server listen port # */

	/* bind (associate) the listen socket number with server IP and port#.
	 * bind is a socket interface function 
	 */
	if(bind(sock,(struct sockaddr *)&svcAddr,sizeof(svcAddr))<0)
	{
		perror("cannot bind");
		close(sock);
		return(ER_BIND_FAILED);	/* bind failed */
	}

	/* 
	 * Set listen queue length to 1 outstanding connection request.
	 * This allows 1 outstanding connect request from client to wait
	 * while processing current connection request, which takes time.
	 * It prevents connection request to fail and client to think server is down
	 * when in fact server is running and busy processing connection request.
	 */
	qlen=1; 


	/* 
	 * Listen for connection request to come from client ftp.
	 * This is a non-blocking socket interface function call, 
	 * meaning, server ftp execution does not block by the 'listen' funcgtion call.
	 * Call returns right away so that server can do whatever it wants.
	 * The TCP transport layer will continuously listen for request and
	 * accept it on behalf of server ftp when the connection requests comes.
	 */

	listen(sock,qlen);  /* socket interface function call */

	/* Store listen socket number to be returned in output parameter 's' */
	*s=sock;

	return(OK); /*successful return */
}


/*
 * sendMessage
 *
 * Function to send specified number of octet (bytes) to client ftp
 *
 * Parameters
 * s		- Socket to be used to send msg to client (input)
 * msg  	- Pointer to character arrary containing msg to be sent (input)
 * msgSize	- Number of bytes, including NULL, in the msg to be sent to client (input)
 *
 * Return status
 *	OK		- Msg successfully sent
 *	ER_SEND_FAILED	- Sending msg failed
 */

int sendMessage(
	int    s,	/* socket to be used to send msg to client */
	char   *msg, 	/* buffer having the message data */
	int    msgSize 	/* size of the message/data in bytes */
	)
{
	int i;


	/* Print the message to be sent byte by byte as character */
	for(i=0; i<msgSize; i++)
	{
		printf("%c",msg[i]);
	}
	printf("\n");

	if((send(s, msg, msgSize, 0)) < 0) /* socket interface call to transmit */
	{
		perror("unable to send ");
		return(ER_SEND_FAILED);
	}

	return(OK); /* successful send */
}


/*
 * receiveMessage
 *
 * Function to receive message from client ftp
 *
 * Parameters
 * s		- Socket to be used to receive msg from client (input)
 * buffer  	- Pointer to character arrary to store received msg (input/output)
 * bufferSize	- Maximum size of the array, "buffer" in octent/byte (input)
 *		    This is the maximum number of bytes that will be stored in buffer
 * msgSize	- Actual # of bytes received and stored in buffer in octet/byes (output)
 *
 * Return status
 *	OK			- Msg successfully received
 *	R_RECEIVE_FAILED	- Receiving msg failed
 */


int receiveMessage (
	int s, 		/* socket */
	char *buffer, 	/* buffer to store received msg */
	int bufferSize, /* how large the buffer is in octet */
	int *msgSize 	/* size of the received msg in octet */
	)
{
	int i;

	*msgSize=recv(s,buffer,bufferSize,0); /* socket interface call to receive msg */

	if(*msgSize<0)
	{
		perror("unable to receive");
		return(ER_RECEIVE_FAILED);
	}

	/* Print the received msg byte by byte */
	for(i=0;i<*msgSize;i++)
	{
		printf("%c", buffer[i]);
	}
	printf("\n");

	return (OK);
}

/*
 * clntConnect
 *
 * Function to create a socket, bind local client IP address and port to the socket
 * and connect to the server
 *
 * Parameters
 * serverName	- IP address of server in dot notation (input)
 * s		- Control connection socket number (output)
 *
 * Return status
 *	OK			- Successfully connected to the server
 *	ER_INVALID_HOST_NAME	- Invalid server name
 *	ER_CREATE_SOCKET_FAILED	- Cannot create socket
 *	ER_BIND_FAILED		- bind failed
 *	ER_CONNECT_FAILED	- connect failed
 */


int clntConnect (
	char *serverName, /* server IP address in dot notation (input) */
	int *s 		  /* control connection socket number (output) */
	)
{
	int sock;	/* local variable to keep socket number */

	struct sockaddr_in clientAddress;  	/* local client IP address */
	struct sockaddr_in serverAddress;	/* server IP address */
	struct hostent	   *serverIPstructure;	/* host entry having server IP address in binary */


	/* Get IP address os server in binary from server name (IP in dot natation) */
	if((serverIPstructure = gethostbyname(serverName)) == NULL)
	{
		printf("%s is unknown server. \n", serverName);
		return (ER_INVALID_HOST_NAME);  /* error return */
	}

	/* Create control connection socket */
	if((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		perror("cannot create socket ");
		return (ER_CREATE_SOCKET_FAILED);	/* error return */
	}

	/* initialize client address structure memory to zero */
	memset((char *) &clientAddress, 0, sizeof(clientAddress));

	/* Set local client IP address, and port in the address structure */
	clientAddress.sin_family = AF_INET;	/* Internet protocol family */
	clientAddress.sin_addr.s_addr = htonl(INADDR_ANY);  /* INADDR_ANY is 0, which means */
						 /* let the system fill client IP address */
	clientAddress.sin_port = 0;  /* With port set to 0, system will allocate a free port */
			  /* from 1024 to (64K -1) */

	/* Associate the socket with local client IP address and port */
	if(bind(sock,(struct sockaddr *)&clientAddress,sizeof(clientAddress))<0)
	{
		perror("cannot bind");
		close(sock);
		return(ER_BIND_FAILED);	/* bind failed */
	}


	/* Initialize serverAddress memory to 0 */
	memset((char *) &serverAddress, 0, sizeof(serverAddress));

	/* Set ftp server ftp address in serverAddress */
	serverAddress.sin_family = AF_INET;
	memcpy((char *) &serverAddress.sin_addr, serverIPstructure->h_addr, 
			serverIPstructure->h_length);
	serverAddress.sin_port = htons(SERVER_FTP_PORT);

	/* Connect to the server */
	if (connect(sock, (struct sockaddr *) &serverAddress, sizeof(serverAddress)) < 0)
	{
		perror("Cannot connect to server ");
		close (sock); 	/* close the control connection socket */
		return(ER_CONNECT_FAILED);  	/* error return */
	}


	/* Store listen socket number to be returned in output parameter 's' */
	*s=sock;

	return(OK); /* successful return */
}  // end of clntConnect() */

//used to connect to the data connection
int dcclntConnect (
	char *serverName, /* server IP address in dot notation (input) */
	int *s 		  /* control connection socket number (output) */
	)
{
	int sock;	/* local variable to keep socket number */

	struct sockaddr_in clientAddress;  	/* local client IP address */
	struct sockaddr_in serverAddress;	/* server IP address */
	struct hostent	   *serverIPstructure;	/* host entry having server IP address in binary */


	/* Get IP address os server in binary from server name (IP in dot natation) */
	if((serverIPstructure = gethostbyname(serverName)) == NULL)
	{
		printf("%s is unknown server. \n", serverName);
		return (ER_INVALID_HOST_NAME);  /* error return */
	}

	/* Create control connection socket */
	if((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		perror("cannot create socket ");
		return (ER_CREATE_SOCKET_FAILED);	/* error return */
	}

	/* initialize client address structure memory to zero */
	memset((char *) &clientAddress, 0, sizeof(clientAddress));

	/* Set local client IP address, and port in the address structure */
	clientAddress.sin_family = AF_INET;	/* Internet protocol family */
	clientAddress.sin_addr.s_addr = htonl(INADDR_ANY);  /* INADDR_ANY is 0, which means */
						 /* let the system fill client IP address */
	clientAddress.sin_port = 0;  /* With port set to 0, system will allocate a free port */
			  /* from 1024 to (64K -1) */

	/* Associate the socket with local client IP address and port */
	if(bind(sock,(struct sockaddr *)&clientAddress,sizeof(clientAddress))<0)
	{
		perror("cannot bind");
		close(sock);
		return(ER_BIND_FAILED);	/* bind failed */
	}


	/* Initialize serverAddress memory to 0 */
	memset((char *) &serverAddress, 0, sizeof(serverAddress));

	/* Set ftp server ftp address in serverAddress */
	serverAddress.sin_family = AF_INET;
	memcpy((char *) &serverAddress.sin_addr, serverIPstructure->h_addr, 
			serverIPstructure->h_length);
	serverAddress.sin_port = htons(DATA_CONNECTION_PORT);

	/* Connect to the server */
	if (connect(sock, (struct sockaddr *) &serverAddress, sizeof(serverAddress)) < 0)
	{
		perror("Cannot connect to server ");
		close (sock); 	/* close the control connection socket */
		return(ER_CONNECT_FAILED);  	/* error return */
	}


	/* Store listen socket number to be returned in output parameter 's' */
	*s=sock;

	return(OK); /* successful return */
}  // end of clntConnect() */


