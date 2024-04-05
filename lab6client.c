/*
* Author: Anthony Wong
* Date: 2024-04-04
* Purpose: Client side code for connecting to a server side socket and receiving file information and then writing to a file.
* Language: C

* Compiler: gcc -o lab6client
* lab6client.c
*/


#include <fcntl.h>
#include <stdlib.h> 
#include <stdio.h> 
#include <sys/types.h>
#include <sys/socket.h> 
#include <arpa/inet.h>
#include <unistd.h>
#include <netinet/in.h> 
#include <errno.h>
#include <string.h> 

typedef enum { false = 0, true = !false } bool;

#define SERVER_ADDRESS      "localhost"
#define PORT_NUM             58129 
#define OUTPUT_FILENAME      "lab_sourcefile_local_clone"

struct Arguments {
  bool f;
  char* filepath;
}; 


void grabArgs( int argc, char *argv[], struct Arguments *args ); 
int error(char* where, char* msg);

/*
* The main function of the program that does the following:
* 1. Creates a client socket
* 2. Connects the client socket to the server address
* 3. Sends the file path to the server
* 4. Receives the file size from the server
* 5. Receives the file from the server and writes it to a file
*/
int main(int argc, char *argv[]) {
  
  int clientSocket; 
  struct sockaddr_in remote_addr;
  char buffer[BUFSIZ]; 
  int remainingData = 0;
  ssize_t len;
  char fileSizeBuffer[BUFSIZ]; 


  struct Arguments args;                      
  grabArgs(argc, argv, &args); 
  
  if(args.f == false){
    error("grabArgs", "Missing -f <filename>");
    return -1;
  }

  memset(&remote_addr, 0, sizeof(remote_addr));

  remote_addr.sin_family = AF_INET; 
  inet_pton(AF_INET, SERVER_ADDRESS, &(remote_addr.sin_addr)); 
  remote_addr.sin_port = htons(PORT_NUM); 

  clientSocket = socket(AF_INET, SOCK_STREAM, 0);
  if (clientSocket == -1) {
    error("ClientSocket create", "Error creating socket...");
  }

  if (connect(clientSocket, (struct sockaddr *)&remote_addr, sizeof(struct sockaddr)) == -1) {
    error("Socket Connect", "Error connecting to server...");
  }

  fprintf(stdout, "Client: sending %s as file path. String size: %d ... \n", args.filepath, strlen(args.filepath));
  if (send(clientSocket, args.filepath, strlen(args.filepath), 0) < 0) {
    error("send filepath", "Could not send the filepath... ");
  }

  recv(clientSocket, fileSizeBuffer, BUFSIZ, 0);
  printf("Client: Expecting filesize of %s \n", fileSizeBuffer);

int receivedFile = open(OUTPUT_FILENAME, O_WRONLY | O_CREAT | O_APPEND, 0644);
if (receivedFile == -1) {
    perror("Open File");
    exit(EXIT_FAILURE);
}

remainingData = atoi(fileSizeBuffer);


/*
* The client will receive the file from the server and write it to a file
* The client will keep receiving data from the server until all the data has been received
* The client will print out the number of bytes received and the number of bytes remaining
*/
while ((remainingData > 0) && ((len = recv(clientSocket, buffer, sizeof(buffer), 0)) > 0)) {
    write(receivedFile, buffer, len);
    remainingData -= len;
    if (remainingData > 0) {
        fprintf(stdout, "Client: Received %d bytes and waiting for:- %d bytes\n", len, remainingData);
    } else {
        fprintf(stdout, "Client: Received %d bytes. Done receiving data from server.\n", len);
        break;
    }
}

fprintf(stdout, "Client: Done writing data from server to file...\n");
close(receivedFile);
close(clientSocket);

  return 0;
}

int error(char* where, char* msg){
  printf("Client Error: (%s) => %s \n", where, msg);
  exit(EXIT_FAILURE); 
}


void grabArgs(int argc, char *argv[], struct Arguments *args) {
  int c;
  extern char *optarg;
  while (optind < argc) {
    if((c = getopt(argc, argv, "f:")) != -1) {
      switch(c) {
        case 'f':
          args->f = true;
          args->filepath = optarg;
          break;
        case '?':
          printf("You gave an unrecognized parameter: -%c. \n", optopt);
          break;
        default:
          printf("Something went wrong... \n");
          break;
      }
    }
  }
}