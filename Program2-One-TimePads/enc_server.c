#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <dirent.h>

void error(const char *msg) { perror(msg); exit(1); } // Error function used for reporting issues

int main(int argc, char *argv[])
{
	int listenSocketFD, establishedConnectionFD, portNumber, fileLength, charsRead, charsSent, totalSent, totalRead, charSum, currIndex;
	socklen_t sizeOfClientInfo;
	pid_t childPID;	
	
	char sendBuffer[1024];
	char readBuffer[1025];
	char convertedMsg[100000];
	char completeMsg[100000];
	char completeKey[100000];
	char clientName[120];

	struct sockaddr_in serverAddress, clientAddress;
	
	if (argc < 2) { fprintf(stderr, "USAGE: %s port\n", argv[0]); exit(1); } // Check usage & args  

	// Set up the address struct for this process (the server)
	memset((char *)&serverAddress, '\0', sizeof(serverAddress)); // Clear out the address struct
	portNumber = atoi(argv[1]); // Get the port number, convert to an integer from a string
	serverAddress.sin_family = AF_INET; // Create a network-capable socket
	serverAddress.sin_port = htons(portNumber); // Store the port number
	serverAddress.sin_addr.s_addr = INADDR_ANY; // Any address is allowed for connection to this process

	// set up the socket
	listenSocketFD = socket(AF_INET, SOCK_STREAM, 0); // Create the socket
	if (listenSocketFD < 0) { fprintf(stderr, "ERROR opening socket"); exit(1); }

	// Enable the socket to begin listening
	if (bind(listenSocketFD, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) {  // bind socket to port
		fprintf(stderr, "ERROR on binding");
		exit(1);
	}

	listen(listenSocketFD, 5); // Flip the socket on - it can now receive up to 5 connections
	
	// declare variables needed for child processes
	int i, j, childExitMethod, connections = 0;
	
	// accept connections infinitely, until manually terminated. Allow up to five child processes/connections
	while (1) {

		if (connections < 5) {

			while ((childPID = waitpid(-1, &childExitMethod, WNOHANG)) > 0) {  //check for child processes to reap
				connections --;
			}

			// Accept a connection, blocking if one is not available until one connects
			sizeOfClientInfo = sizeof(clientAddress); // Get the size of the address for the client that will connect
			establishedConnectionFD = accept(listenSocketFD, (struct sockaddr*)&clientAddress, &sizeOfClientInfo); // Accept
			if (establishedConnectionFD < 0) { fprintf(stderr, "ERROR on accept"); exit(1); }

			// set default values for processes
			pid_t spawnPid = -5;
		

			// fork new child process after accepting connection
			spawnPid = fork();

			// if something goes wrong with fork, print issue to stderr	
			if (spawnPid == 1) { fprintf(stderr, "enc_server.c: There was an error on fork #: %d", i); exit(1); }
			else if (spawnPid == 0) {

				// clear char arrays for use
				memset(clientName, '\0', sizeof(clientName));
				memset(completeMsg, '\0', sizeof(completeMsg));
				memset(readBuffer, '\0', sizeof(readBuffer));
				memset(sendBuffer, '\0', sizeof(sendBuffer));

				// get client name
				charsRead = recv(establishedConnectionFD, readBuffer, sizeof(readBuffer) - 1, 0); // Read the client's message from the socket
				if (charsRead < 0) fprintf(stderr, "ERROR reading from socket");
				//printf("SERVER: I received this from the client: \"%s\"\n", readBuffer);
				//fflush(stdout);

				// copy input
				for (i = 0; i < strlen(readBuffer); i++) {
					clientName[i] = readBuffer[i];
				}

				// clean buffer			
				memset(readBuffer, '\0', sizeof(readBuffer));

				if (strcmp(clientName, "enc_client") == 0) {

					// send confirmation to client
					charsSent = send(establishedConnectionFD, "Client connection was accepted", 31, 0); // Send success back
					if (charsSent < 0) fprintf(stderr, "ERROR reading from socket");

					// get file length from client
					charsRead = recv(establishedConnectionFD, readBuffer, sizeof(readBuffer) - 1, 0); // Read the client's message from the socket
					if (charsRead < 0) fprintf(stderr, "ERROR reading from socket");
					//printf("SERVER: I received this from the client: \"%s\"\n", readBuffer);
					//fflush(stdout);

					// send confirmation to client
					charsSent = send(establishedConnectionFD, "I got your file length", 23, 0); // Send success back
					if (charsSent < 0) fprintf(stderr, "ERROR reading from socket");

					// convert file length to int, store
					fileLength = atoi(readBuffer);

					// reset msg length counter
					totalRead = 0;

					// Get the plain text message from the client and store it. Read until delimiter is found
					while (totalRead < fileLength) {
						// reset buffer for next round of reading
						memset(readBuffer, '\0', sizeof(readBuffer));
						charsRead = recv(establishedConnectionFD, readBuffer, sizeof(readBuffer) - 1, 0); // Read the client's message from the socket		
						totalRead += charsRead;  //count char read for total message length

						//printf("SERVER: I received this from the client: \"%s\"\n", readBuffer);
						//fflush(stdout);

						// send confirmation to client
						//charsSent = send(establishedConnectionFD, "I got your data", 16, 0); // Send success back
						//if (charsSent < 0) fprintf(stderr, "ERROR writing to socket");

						strcat(completeMsg, readBuffer);
						if (charsRead == -1 || charsRead == 0) { break; }
					}

					// reset msg length counter
					totalRead = 0;

					// Get the key from the client and store it. Read until delimiter is found
					while (totalRead < fileLength) {
						// reset buffer for next round of reading
						memset(readBuffer, '\0', sizeof(readBuffer));
						charsRead = recv(establishedConnectionFD, readBuffer, sizeof(readBuffer) - 1, 0); // Read the client's message from the socket
						totalRead += charsRead;

						//printf("SERVER: I received this from the client: \"%s\"\n", readBuffer);
						//fflush(stdout);
						
						// send confirmation to client
						//charsSent = send(establishedConnectionFD, "I got your data", 16, 0); // Send success back
						//if (charsSent < 0) fprintf(stderr, "ERROR writing to socket");

						strcat(completeKey, readBuffer);
						if (charsRead == -1 || charsRead == 0) { break; }
					}

					// encrypt plaintext message using key, minus newline
					for (i = 0; i < fileLength - 1; i++) {
						// need to change any space char to @(value: 64) in order to do modulus math correctly
						if (completeMsg[i] == ' ') {
							completeMsg[i] = '@';
						}

						if (completeKey[i] == ' ') {
							completeKey[i] = '@';
						}

						// convert char to range 0 - 26 for math (modulus 27)
						completeMsg[i] -= 64;
						completeKey[i] -= 64;

						// add key and txt char, perform modulus operation	
						charSum = (completeMsg[i] + completeKey[i]);

						if (charSum > 26) {
							charSum = charSum % 27;
						}

						convertedMsg[i] = charSum;

						// add 64 back to convert to original upper-case alpha char
						convertedMsg[i] += 64;

						// convert any @ char back to space char
						if (convertedMsg[i] == '@') {
							convertedMsg[i] = ' ';
						}

					}

					// send the encrypted message back to client
					memset(sendBuffer, '\0', sizeof(sendBuffer));
					currIndex = 0;
					totalSent = 0;

					// break up message by buffer length, send in chunks, subtract newline from file length
					while (totalSent < (fileLength - 1)) {
						for (i = 0; i < 1024; i++) {
							sendBuffer[i] = convertedMsg[currIndex];
							currIndex++;

							if (currIndex == (fileLength - 1)) {
								break;
							}
						}

						// write current chunk to client
						charsSent = send(establishedConnectionFD, sendBuffer, strlen(sendBuffer), 0);
						totalSent += charsSent;

						// error checking
						if (charsSent < 0) fprintf(stderr, "ERROR writing to socket");
						if (charsSent < strlen(sendBuffer)) printf("SERVER: WARNING: Not all data written to socket!\n");

						//confirm receipt
						//memset(readBuffer, '\0', sizeof(readBuffer));
						//charsRead = recv(establishedConnectionFD, readBuffer, sizeof(readBuffer) - 1, 0); // Read data from the socket, leaving \0 at end
						//if (charsRead < 0) error("SERVER: ERROR reading from socket");

						//printf("SERVER: I received this from the client: \"%s\"\n", readBuffer);
						//fflush(stdout);

						// clear buffer for next round of sending
						memset(sendBuffer, '\0', sizeof(sendBuffer));
					}

					// wrap up after successfully finishing work in child process
					close(establishedConnectionFD); // Close socket
					exit(0);
				}

				else {
					charsRead = send(establishedConnectionFD, "Client connect is not accepted", 31, 0);
					if (charsRead < 0) fprintf(stderr, "ERROR reading from socket");
					exit(2);
				}

			}

			else {
				connections++;  // increment connection count
			}
		}

		else {

			if ((childPID = waitpid(-1, &childExitMethod, 0)) > 0) {  // if connection limit reached, wait here for processes to reap
				connections--;
			}
		}
 	}
	
	close(listenSocketFD); // Close the listening socket
	return 0; 
}
