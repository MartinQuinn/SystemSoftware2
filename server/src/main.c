
// C13477058 Martin Quinn
// Systems Software, Assignment 2

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
#include "connection_handler.h"

int main(int argc , char *argv[]) {
  int socket_desc, client_sock, c , *new_sock;
  struct sockaddr_in server, client;

  // Creating the server's socket
  socket_desc = socket(AF_INET, SOCK_STREAM, 0);
  if (socket_desc == -1) {
    puts("Could not create socket");
  }
  puts("Socket created");

  // Prepare the sockaddr_in structure
  server.sin_family = AF_INET; // Protocol Family
  server.sin_addr.s_addr = INADDR_ANY; // AutoFill local address
  server.sin_port = htons(8888); // server port number

  // Bind() associates the socket with its local address
  if(bind(socket_desc,(struct sockaddr *) &server, sizeof(server)) < 0) {
    perror("Bind failed. Error");
    return 1;
  }
  puts("Bind completed");
  listen(socket_desc , 3);

  // Accept incoming connections
  puts("Awaiting incoming connection to the server..");
  c = sizeof(struct sockaddr_in);
  while((client_sock = accept(socket_desc, (struct sockaddr *) &client, (socklen_t*)&c))) {
    puts("\nConnection accepted");

    // pthread_t is our mutex.
    pthread_t sniffer_thread;
    new_sock = malloc(1);
    *new_sock = client_sock;

    if(pthread_create(&sniffer_thread, NULL, connection_handler, (void*) new_sock) < 0) {
      perror("Could not create thread");
      return 1;
    }
  }
  if (client_sock < 0) {
    perror("Connection request failed");
    return 1;
  }
  return 0;
}
