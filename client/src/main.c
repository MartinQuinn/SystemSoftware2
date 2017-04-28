
// Martin Quinn
// Systems Software, Assignment 2

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define MSG_SIZE 100
#define PATH_SIZE 500
#define FILE_SIZE 512
#define LOCAL_STORAGE "../storage/"

int main(int argc, char *argv[]) {
  int sock;
  struct sockaddr_in server;
  char option[2], response[10], message[MSG_SIZE], server_reply[MSG_SIZE], file_dir[PATH_SIZE], username[10], password[10];

  // Create socket
  sock = socket(AF_INET, SOCK_STREAM, 0);
  if (sock == -1) {
    puts("Could not create socket");
  }

  // Setting server configuration
  server.sin_addr.s_addr = inet_addr("0.0.0.0");
  server.sin_family = AF_INET;
  server.sin_port = htons(8888);

  // Connect to remote server
  if (connect(sock, (struct sockaddr *)&server, sizeof(server)) < 0) {
    perror("connect failed. Error");
    return 1;
  }

  // Login user
  puts("Connected to server\n");
  send(sock, "INIT_LOGIN", strlen("INIT_LOGIN"), 0);

  // Username input
  printf("Username: ");
  scanf("%s", username);
  send(sock, username, strlen(username), 0);

  // Password input
  printf("Password: ");
  scanf("%s", password);
  send(sock, password, strlen(password), 0);

  // Receive server authentication response
  recv(sock, response, 10, 0);

  if (strcmp(response, "200") == 0) {
    puts("\nLogin successful\n");
  } else {
    puts("\nLogin unsuccessful\n");
    return 1;
  }

  while(1) {
    puts("Menu:\n 1: Transfer file \n 2: Logout \n Make a selection: ");
    scanf("%s", option);

    if (strcmp(option, "1") == 0) {

      // Initiate transfer
      puts("\nStarting transfer");
      send(sock, "INIT_TRANSFER", strlen("INIT_TRANSFER"), 0);
      puts("\nEnter a filename: \n");
      scanf("%s", message);
      strcpy(file_dir, LOCAL_STORAGE);
      strcat(file_dir, message);

      if(access(file_dir, F_OK) == -1 ) {
        puts("That file doesn't exist");
        return 1;
      }

      send(sock, message, strlen(message), 0);
      puts("\nChoose a path: \n 1: Root \n 2: Sales \n 3: Promotions \n 4: Offers \n 5: Marketing \n Input your choice here: ");
      scanf("%s", option);

      // If Else used for choosing destination directory
      if (strcmp(option, "1") == 0) {
        send(sock, "/", strlen("/"), 0);
      } else if (strcmp(option, "2") == 0) {
        send(sock, "sales", strlen("sales"), 0);
      } else if (strcmp(option, "3") == 0) {
        send(sock, "promotions", strlen("promotions"), 0);
      } else if (strcmp(option, "4") == 0) {
        send(sock, "offers", strlen("offers"), 0);
      } else if (strcmp(option, "5") == 0) {
        send(sock, "marketing", strlen("marketing"), 0);
      } else {
        puts("Input was not an option");
        return 1;
      }

      // Preparing for the file transfer
      char file_buffer[FILE_SIZE];
      memset(file_buffer, 0, sizeof(file_buffer));
	    int block_size, i = 0;
	    printf("\nSending %s to server... \n", file_dir);
	    FILE *file_open = fopen(file_dir, "r");

      // Begin transfer of file. File must not be empty.
	    while((block_size = fread(file_buffer, sizeof(char), FILE_SIZE, file_open)) > 0) {

    		printf("Data sent %d = %d\n", i, block_size);

    		if (send(sock, file_buffer, block_size, 0) < 0) {
            return 1;
        }
    		memset(file_buffer, 0, sizeof(file_buffer));
    		i++;
      }

      puts("File sent.");
      recv(sock, response, 10, 0);

      if (strcmp(response, "OK") == 0) {
        puts("Transfer successful\n");
      } else {
        puts("Transfer failure\n");
        close(sock);
        return 1;
      }
      fclose(file_open);

    // Close the connection
    } else if (strcmp(option, "2") == 0) {
      close(sock);
      return 0;
    } else {
      puts("Connection not closed");
    }
  }// End 1. file transfer
} // End all
