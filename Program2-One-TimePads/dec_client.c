#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 

void error(const char *msg) { perror(msg); exit(0); } // Error function used for reporting issues

int main(int argc, char* argv[])
{
	int i, j, currIndex, modResult, socketFD, portNumber, totalSent, totalRead, charsSent, charsRead, cipherFD, cipherLen, keyFD, keyLen, msgLen;
	struct sockaddr_in serverAddress;
	struct hostent* serverHostInfo;
	char* clientName = "dec_client";
	char readBuffer[1025];
	char sendBuffer[1024];
	char cipherText[100000];
	char keyText[100000];
	char encryptedMsg[100000];

	// clear buffers for use later
	memset(readBuffer, '\0', sizeof(readBuffer));
	memset(sendBuffer, '\0', sizeof(sendBuffer));
	memset(cipherText, '\0', sizeof(cipherText));
	memset(keyText, '\0', sizeof(keyText));
	memset(encryptedMsg, '\0', sizeof(encryptedMsg));

	// if improper number of arguments
	if (argc != 4) { fprintf(stderr, "Improper number of arguments passed: %d\n", argc); exit(1); } // check for right number of arguments

	// Setup the server address struct
	memset((char*)&serverAddress, '\0', sizeof(serverAddress)); // Clear out the address struct

	portNumber = atoi(argv[3]); // Get the port number, convert to an integer from a string
	serverAddress.sin_family = AF_INET; // Create a network-capable socket
	serverAddress.sin_port = htons(portNumber); // Store the port number
	serverHostInfo = gethostbyname("localhost"); // set host to be localhost, convert to proper formati
	if (serverHostInfo == NULL) { fprintf(stderr, "CLIENT: ERROR, no such host\n"); exit(0); }
	memcpy((char*)&serverAddress.sin_addr.s_addr, (char*)serverHostInfo->h_addr, serverHostInfo->h_length); // Copy in the address

	// Set up the socket
	socketFD = socket(AF_INET, SOCK_STREAM, 0); // Create the socket
	if (socketFD < 0) { fprintf(stderr, "CLIENT: ERROR opening socket"); exit(1); }

	// Connect to server
	if (connect(socketFD, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) { // Connect socket to address
		fprintf(stderr, "ERROR: could not contact %s on port %d\n", clientName, portNumber);
		exit(1);
	}

	// send name of client for verification
	charsSent = send(socketFD, clientName, strlen(clientName), 0);
	if (charsSent < 0) fprintf(stderr, "ERROR reading from socket");

	//confirm receipt
	charsRead = recv(socketFD, readBuffer, sizeof(readBuffer) - 1, 0); // Read data from the socket, leaving \0 at end
	if (charsRead < 0) error("CLIENT: ERROR reading from socket");
	//printf("CLIENT: I received this from the server: \"%s\"\n", readBuffer);
	//fflush(stdout);

	
	if (strcmp(readBuffer, "Client connection was accepted") == 0) {

		// open ciphertext file to validate input	
		cipherFD = open(argv[1], O_RDONLY);

		// if unable to open, send error message
		if (cipherFD < 0) { fprintf(stderr, "Error. Cannot open ciphertext file: %s\n", argv[1]); exit(1); }

		// read the ciphertext file, count chars read
		cipherLen = read(cipherFD, cipherText, sizeof(cipherText));

		// read input from ciphertext, minus newline char
		for (i = 0; i < cipherLen - 1; i++) {
			// if not upper-case alphabet or space, send error msg
			if ((cipherText[i] > 90 || cipherText[i] < 65) && (cipherText[i] != 32)) {
				fprintf(stderr, "Error. Ciphertext file: %s, contains a bad character: %c\n", argv[1], cipherText[i]);
				exit(1);
			}
		}

		// close FD for cipher text file after validating
		close(cipherFD);
		
		// open key to validate input
		keyFD = open(argv[2], O_RDONLY);

		// if unable to open, send error message
		if (keyFD < 0) { fprintf(stderr, "Error. Cannot open key: %s\n", argv[2]); exit(1); }

		// read the key file, count chars read
		keyLen = read(keyFD, keyText, sizeof(keyText));

		// read input from key, minus newline char
		for (i = 0; i < keyLen - 1; i++) {
			// if not upper-case alphabet or space, send error msg
			if ((keyText[i] > 90 || keyText[i] < 65) && (keyText[i] != 32)) {
				fprintf(stderr, "Error. Key file: %s, contains a bad character: %c\n", argv[2], keyText[i]);
				exit(1);
			}
		}

		// close key text FD after validating
		close(keyFD);
		
		// compare char count in keyfile vs ciphertext file, cipher text can't be longer than key, if is send error
		if (keyLen < cipherLen) { 
			fprintf(stderr, "Error. Key file: %s, is shorter than ciphertext file: %s\n", argv[2], argv[1]); 
			exit(1); 
		}
		
		// send file length to server
		memset(sendBuffer, '\0', sizeof(sendBuffer));
		sprintf(sendBuffer, "%d", cipherLen);
		charsSent = send(socketFD, sendBuffer, strlen(sendBuffer), 0);
		if (charsSent < 0) fprintf(stderr, "ERROR reading from socket");
		
		//confirm receipt
		memset(readBuffer, '\0', sizeof(readBuffer));
		charsRead = recv(socketFD, readBuffer, sizeof(readBuffer) - 1, 0); // Read data from the socket, leaving \0 at end
		if (charsRead < 0) error("CLIENT: ERROR reading from socket");
		//printf("CLIENT: I received this from the server: \"%s\"\n", readBuffer);
		//fflush(stdout);

		// reset buffer and index, figure out how many chunks to send message in
		memset(sendBuffer, '\0', sizeof(sendBuffer));
		currIndex = 0;
		totalSent = 0;

		while (totalSent < cipherLen) {
			for (i = 0; i < 1024; i++) {
				sendBuffer[i] = cipherText[currIndex];
				currIndex++;

				if (currIndex == cipherLen) {
					break;
				}
			}

			// write current chunk to server
			charsSent = send(socketFD, sendBuffer, strlen(sendBuffer), 0);
			totalSent += charsSent;

			// error checking
			if (charsSent < 0) fprintf(stderr, "ERROR writing to socket");
			if (charsSent < strlen(sendBuffer)) printf("CLIENT: WARNING: Not all data written to socket!\n");

			//confirm receipt
			//memset(readBuffer, '\0', sizeof(readBuffer));
			//charsRead = recv(socketFD, readBuffer, sizeof(readBuffer) - 1, 0); // Read data from the socket, leaving \0 at end
			//if (charsRead < 0) error("CLIENT: ERROR reading from socket");
			//printf("CLIENT: I received this from the server: \"%s\"\n", readBuffer);
			//fflush(stdout);

			// clear buffer for next round of sending
			memset(sendBuffer, '\0', sizeof(sendBuffer));
		}

		// reset buffer and index, figure out how many chunks to send message in
		memset(sendBuffer, '\0', sizeof(sendBuffer));
		currIndex = 0;
		totalSent = 0;

		// send key text to server
		while (totalSent < cipherLen) {
			for (i = 0; i < 1024; i++) {
				sendBuffer[i] = keyText[currIndex];
				currIndex++;

				if (currIndex == cipherLen) {
					break;
				}
			}

			// write current chunk to server
			charsSent = send(socketFD, sendBuffer, strlen(sendBuffer), 0);
			totalSent += charsSent;

			// error checking
			if (charsSent < 0) fprintf(stderr, "ERROR writing to socket");
			if (charsSent < strlen(sendBuffer)) printf("CLIENT: WARNING: Not all data written to socket!\n");

			//confirm receipt
			//memset(readBuffer, '\0', sizeof(readBuffer));
			//charsRead = recv(socketFD, readBuffer, sizeof(readBuffer) - 1, 0); // Read data from the socket, leaving \0 at end
			//if (charsRead < 0) error("CLIENT: ERROR reading from socket");
			//printf("CLIENT: I received this from the server: \"%s\"\n", readBuffer);
			//fflush(stdout);

			// clear buffer for next round of sending
			memset(sendBuffer, '\0', sizeof(sendBuffer));
		}

		memset(readBuffer, '\0', sizeof(readBuffer));
		totalRead = 0;

		// Get the encrypted message from the server and store it. Subtract newline char
		while (totalRead < (cipherLen - 1)) {
			// reset buffer for next round of reading
			memset(readBuffer, '\0', sizeof(readBuffer));
			charsRead = recv(socketFD, readBuffer, sizeof(readBuffer) - 1, 0); // Read the client's message from the socket
			totalRead += charsRead;
			
			//printf("CLIENT: I received this from the server: \"%s\"\n", readBuffer);
			//fflush(stdout);
			
			// send confirmation to client
			//charsSent = send(socketFD, "I got your data", 16, 0); // Send success back
			//if (charsSent < 0) fprintf(stderr, "ERROR writing to socket");
			
			strcat(encryptedMsg, readBuffer);
			if (charsRead == -1 || charsRead == 0) { break; }
		}

		printf("%s\n", encryptedMsg);
		fflush(stdout);

		close(socketFD); // Close the socket
		return 0;
	}

	else {
		// if wrong server, disconnect
		fprintf(stderr, "Tried connecting to wrong server on port: %d. Disconnecting...\n", portNumber);
		close(socketFD);  // close socket and exit out
		exit(2);
		
	}

}
