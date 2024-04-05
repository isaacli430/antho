/*
* Author: Anthony Wong
* Date: 2024-04-04
* Purpose: Server side code for connecting to a client side socket and sending file information and then writing to a file. 
*  The server will listen for a connection from the client and then send the file size of the file requested by the client.
* Language: C

* Compiler: gcc -o lab6server
*lab6server.c
*/

/*
*   Includes library files for the program
*  stdio.h - Standard Input Output
*  sys/types.h - System Types
*  sys/socket.h - System Socket
*  stdlib.h - Standard Library
*  errno.h - Error Number
*  fcntl.h - File Control
*  sys/sendfile.h - System Send File
*  string.h - String
*  arpa/inet.h - ARPA Internet
*  unistd.h - Unix Standard
*  netinet/in.h - Network Internet
*  sys/stat.h - System Stat
*/
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <errno.h> 
#include <fcntl.h> 
#include <sys/sendfile.h> 
#include <string.h> 
#include <arpa/inet.h> 
#include <unistd.h>
#include <netinet/in.h> 
#include <sys/stat.h> 

/*
* SERVER_ADDRESS - IP Address of the server
* PORT_NUM - Port number of the server
* BLOCK_SIZE - Size of the block to send
*/
#define SERVER_ADDRESS      "localhost" 
#define PORT_NUM             58129
#define BLOCK_SIZE           1024

/*
* Function Prototypes  
* error - Function to print out error messages
*/
int error(char* where, char* msg);

/*
* The main function of the program that does the following:
* 1. Creates a server socket
* 2. Binds the server socket to the server address
* 3. Listens for a connection from the client
* 4. Accepts the connection from the client
* 5. Receives the file path from the client
* 6. Opens the file
* 7. Sends the file size to the client
* 8. Sends the file data to the client
* 9. Closes the file
* 10. Closes the server socket
* 11. Exits the program
* The error function is called if there is an error in the program
* @param argc - Number of arguments
* @param argv - Arguments
* @return - Returns 0 if the program runs successfully
*/
int main(int argc, char *argv[]) {
  int serverSocket;
  int peerSocket; 
  socklen_t socketLength; 
  struct sockaddr_in serverAddress; 
  struct sockaddr_in peerAddress;

  ssize_t len; 
  int fd; 
  int sentBytes = 0; 
  char fileSize[256];
  struct stat fileStat;
  off_t offset; 
  int remainingData; 
  int bytesRead;
  char buffer[1024];

  serverSocket = socket(AF_INET, SOCK_STREAM, 0);
  if (serverSocket == -1) {
    error("Socket Creation", "Error creating socket");
  }

/*
* The server will bind the server socket to the server address
* if the server socket cannot be bound, an error will be thrown
* The server address is set to the server address and port number
* The server will listen for a connection from the client and then send the file size of the file requested by the client.
* The server will receive the file path from the client
* if filepath is not received, an error will be thrown
*/
  memset(&serverAddress, 0, sizeof(serverAddress)); 
  serverAddress.sin_family = AF_INET;
  inet_pton(AF_INET, SERVER_ADDRESS, &(serverAddress.sin_addr)); 
  serverAddress.sin_port = htons(PORT_NUM); 

  if ((bind(serverSocket, (struct sockaddr *)&serverAddress, sizeof(struct sockaddr))) == -1) {
    error("Socket Bind", "Error on bind");
  }

  if ((listen(serverSocket, 5)) == -1) {
    error("Socket Listen", "Error on listen");
  }

  /*
  * The server will listen for a connection from the client and then send the file size of the file requested by the client.
  */
  while(1) {
    socketLength = sizeof(struct sockaddr_in);
    peerSocket = accept(serverSocket, (struct sockaddr *)&peerAddress, &socketLength);
    if (peerSocket == -1) {
      error("Socket Accept", "Error on accept");
    }
    fprintf(stdout, "Server: Accepted connection --> %s\n", inet_ntoa(peerAddress.sin_addr));

    /*
    * The server will receive the file path from the client
    * if filepath is not received, an error will be thrown
    */
    char filepathBuffer[BUFSIZ];
    int filePathlength = recv(peerSocket, filepathBuffer, BUFSIZ, 0); 
    if(filePathlength < 0){
      error("Missing Filepath", "No file path recieved from client...");
    } else {
      fprintf(stdout, "Server: File to retrieve for client: %s \n", filepathBuffer);
    }

    /*
    * The server will open the file
    * if the file cannot be opened, an error will be thrown
    */
    fd = open(filepathBuffer, O_RDONLY);
    if (fd == -1) {
      error("File Open", "Error opening file");
    }

    /*
    * The server will send the file size to the client
    * if the file size cannot be sent, an error will be thrown
    */
    if (fstat(fd, &fileStat) < 0) {
      error("File Stat", "Error on filestat");
    }

    fprintf(stdout, "Server: File Size: %d bytes\n", fileStat.st_size);
    sprintf(fileSize, "%d", fileStat.st_size); 

    /*
    * The server will send the file content to the client
    * if the file size cannot be sent, an error will be thrown
    */
    len = send(peerSocket, fileSize, sizeof(fileSize), 0);
    if (len < 0) {
      error("File Send", "Error on sending file");
    }

    offset = 0; 
    remainingData = fileStat.st_size; 
    int dataSizeToSend = BLOCK_SIZE; 

    while (remainingData > 0) {
    bytesRead = read(fd, buffer, sizeof(buffer));
    if (bytesRead <= 0) {
        break;
    }

    /*
    * The server will send the file data to the client
    * if the file data cannot be sent, an error will be thrown
    */
    sentBytes = write(peerSocket, buffer, bytesRead);
    fprintf(stdout, "Server: sent %d bytes from file's data, offset is now : %d and remaining data = %d\n", sentBytes, offset, remainingData);
    remainingData -= sentBytes;

    /*
    * if the remaining data is less than the data size to send, the data size to send will be updated
    */
    if(remainingData < dataSizeToSend) {
        printf("New data size to send: %d \n", dataSizeToSend);
        dataSizeToSend = remainingData;
    }
}

    printf("Server: Done sending data... \n");
  }
}

/*
* Function to print out error messages
*/
int error(char* where, char* msg){
  printf("Server Error: (%s) => %s \n", where, msg);
  exit(EXIT_FAILURE); 
}