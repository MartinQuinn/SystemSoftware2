
// C13477058 Martin Quinn
// Systems Software, Assignment 2

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>

#define MSG_SIZE 100
#define PATH_SIZE 500
#define FILE_SIZE 512
#define WEBSITE_DIR "../website"
#define LOG_DIR "../storage/log.txt"
#define CRED_DIR "../storage/credentials.txt"

// Mutex to prevent file locking
pthread_mutex_t lock_x;

void *connection_handler(void *socket_desc) {
  int sock = *(int*)socket_desc;
  int read_size, bytes;
  char *message, client_message[MSG_SIZE], client_filename[MSG_SIZE],
  client_filepath[MSG_SIZE], server_filepath[PATH_SIZE], file_buffer[FILE_SIZE],
  username[50], password[50];

  // Receive message from client
  while((read_size = recv(sock, client_message, MSG_SIZE, 0)) > 0) {

    // Open CRED file and check client credentials
    if (strcmp(client_message, "INIT_LOGIN") == 0) {
      puts("\nLogin started");
      int found = 0;
      char user_auth[50], pass_auth[50];

      recv(sock, username, 50, 0);
      recv(sock, password, 50, 0);

      pthread_mutex_lock(&lock_x);

      FILE *cred_file = fopen(CRED_DIR, "r");

      char line[80];

      while(fgets(line, 80, cred_file)) {
        sscanf(line, "username: %s password: %s", user_auth, pass_auth);

        if((strcmp(username, user_auth) == 0) && (strcmp(password, pass_auth) == 0)) {
          found = 1;
        }
      }

      fclose(cred_file);
      pthread_mutex_unlock(&lock_x);

      if (found == 1) {
        send(sock, "200", sizeof("200"), 0);
      } else {
        send(sock, "401", sizeof("401"), 0);
        puts("Login aborted");
        free(socket_desc);
        pthread_exit(NULL);
      }

      puts("Login completed\n");
    }

    // Read client message
    if (strcmp(client_message, "INIT_TRANSFER") == 0) {
      puts("Transfer started");

      // Recieve filename
      if ((bytes = recv(sock, client_filename, MSG_SIZE, 0)) == 0){
        puts("No data recieved");
        free(socket_desc);
        pthread_exit(NULL);
      }

      // Recieve path
      if ((bytes = recv(sock, client_filepath, MSG_SIZE, 0)) == 0) {
        puts("No data recieved");
        free(socket_desc);
        pthread_exit(NULL);
      }

      // Create server filepath
      strcpy(server_filepath, WEBSITE_DIR);
      strcat(server_filepath, "/");
      strcat(server_filepath, client_filepath);
      strcat(server_filepath, "/");
      strcat(server_filepath, client_filename);
      puts(server_filepath);

      // Lock shared resource
      pthread_mutex_lock(&lock_x);

      // Recieve file
      FILE *file_open = fopen(server_filepath, "w");

      memset(file_buffer, 0, sizeof(file_buffer));
      int bytes_recv = 0;
      int i = 0;
      while((bytes_recv = recv(sock, file_buffer, FILE_SIZE, 0)) > 0) {
        printf("Data Received %d = %d\n", i, bytes_recv);
        int write_sz = fwrite(file_buffer, sizeof(char), bytes_recv, file_open);
        memset(file_buffer, 0, sizeof(file_buffer));
        i++;

        if (write_sz == 0 || write_sz != 512) {
            break;
        }
      }

      FILE *logging;
      time_t rawtime;
      struct tm * timeinfo;
      time ( &rawtime );
      timeinfo = localtime ( &rawtime );

      logging = fopen(LOG_DIR, "a+"); // a+ (create + append) option will allow appending which is useful in a log file
      if (logging == NULL) {
        puts("Logging incomplete!\n");
      }
      fprintf(logging, "-----------------------------------------------------------\n");
      fprintf(logging, "User: %s modified %s at %s", username, client_filename, asctime (timeinfo));
      fprintf(logging, "-----------------------------------------------------------\n\n");
      puts("Logging complete!\n");
      puts("Transfer completed\n");


      send(sock, "OK", sizeof("OK"), 0);
      fclose(logging);
      fclose(file_open);

      // Unlock shared resource
      pthread_mutex_unlock(&lock_x);
    }
  }

  if(read_size == 0) {
    puts("Client disconnected");
    fflush(stdout);
  }
  else if(read_size == -1) {
    perror("recv failed");
  }

  // Free the socket to
  free(socket_desc);

  pthread_exit(NULL);
}
