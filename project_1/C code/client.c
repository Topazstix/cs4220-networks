/* This program is designed to represent a client server in C. It is setup to communicate securely via open SSL. The client will encrypt and send a message 
after validating certificates with the given server. */


// Included
#include <stdio.h>
#include <errno.h>
#include <stdlib.h> // *-NIX based sys's & atoi convt str -> int
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

// Define
#define MAX_STRING_LENGTH 80
#define ERROR -1

// Prototype for compiler to grasp method
void fgetsTrim(char* string, int length, FILE* stream);

// Functions for the connection
/* Function to open the connection for the client.
Arguments: the hostname passed as a constant string and the port number as an integer
Returns: the socket file descriptor
*/
int OpenConnection(const char *hostname, int port) {
  
  // Get and verify the hostname
  struct hostent *host = gethostbyname(hostname);
  if (!host) {
    perror("ERROR: HOSTNAME IS INVALID");
    abort();
  }

  // Create the socket
  int sockfd = socket(AF_INET, SOCK_STREAM, 0);
  // Check/handle socket status
  if (sockfd < 0) {
    perror("ERROR: SOCKET CREATION FAILED");
    abort();
  }

  // Prepare for connection
  struct sockaddr_in addr = {0};
  addr.sin_family = AF_INET;
  addr.sin_port = htons(port);
  addr.sin_addr.s_addr = *(long*)(host->h_addr_list[0]);

  // Connect and check connection
  if (connect(sockfd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
    perror("ERROR: CANNOT CONNECT TO SERVER");
    abort();
  }

  // Return socket file descriptor
  return sockfd;
}

/* Function to show the certificate of the server
Arguments: An SSL pointer that represents an SSL connection
Returns: Nothing
*/
void showingCertifications(SSL *ssl) {
  
  // Grab the server's certification via ssl stream
  X509 *cert = SSL_get_peer_certificate(ssl);
  
  // If no cert, err and close client
  if (!cert) {
    printf("ERROR: No server certificate presented\n");
    abort();
  } else {
    // Pull and display certificate information
    char *subject = X509_NAME_oneline(X509_get_subject_name(cert), 0, 0);
    char *issuer  = X509_NAME_oneline(X509_get_issuer_name(cert),  0, 0);
    printf("Server certificate subject: %s\n", subject);
    printf("Server certificate issuer : %s\n", issuer);
    
    // Free memory :)
    OPENSSL_free(subject);
    OPENSSL_free(issuer);
    X509_free(cert);
  }
}

/* Function to setup SSL_CTX 
Arguments: Path to the CA-certificate in local
Returns: a pointer to an SSL_CTX structure containing information about the connection
*/
SSL_CTX* setupCTX(const char *ca_cert_path) {
  
  // Load algos w/ errors
  OpenSSL_add_all_algorithms();
  SSL_load_error_strings();

  // Create context using a generic TLS client method
  const SSL_METHOD *method = TLS_client_method();
  SSL_CTX *ctx = SSL_CTX_new(method);
  
  // If bad connection, close and error
  if (!ctx) {
    ERR_print_errors_fp(stderr);
    abort();
  }

  // Preload CA to verify connection
  if (SSL_CTX_load_verify_locations(ctx, ca_cert_path, NULL) != 1) {
    fprintf(stderr, "ERROR: Cannot load CA file from path: %s\n", ca_cert_path);
    ERR_print_errors_fp(stderr);
    abort();
  }

  // Force server to provide a valid certificate
  SSL_CTX_set_verify(ctx, SSL_VERIFY_PEER, NULL);

  return ctx;
}

/* Function to handle graceful closing of ssl streams where errors may occur
Arguments: 
  - ssl: ssl stream object
  - server_fd: TCP socket stream object
  - ctx: SSL_CTX object containing connection info on the SSL stream
Returns: 0
*/
int badConnection(SSL *ssl, SSL_CTX *ctx, int server_fd){
  SSL_free(ssl);
  close(server_fd);
  SSL_CTX_free(ctx);
  return 0;
}

/* Main function to simulate the client sending a request to a server.
Arguments: Gets the number of arguments as the first argument and the rest of the arguments as an array of strings.
Returns: idk yet nothing important probably just an exit code.
*/
int main(int argc, char *argv[]) {
  
  // Check to see if the program was called with the right number of arguments.
  if (argc != 4) {
    fprintf(stderr, "Usage: %s <path/to/ca.pem> <hostname> <port>\n", argv[0]);
    return 1;
  }

  // Setup for SSL 
  char *ca_path = argv[1];
  const char *hostname = argv[2];
  int portnumber = atoi(argv[3]);  // convert string → int

  SSL_library_init();

  // Setup OpenSSL context
  SSL_CTX *ctx = setupCTX(ca_path);

  // Open a TCP connection
  int server_fd = OpenConnection(hostname, portnumber);

  // Create SSL stream, attach to socket, perform handshake
  SSL *ssl = SSL_new(ctx);
  SSL_set_fd(ssl, server_fd);
  if (SSL_connect(ssl) != 1) {
    ERR_print_errors_fp(stderr);
    badConnection(ssl, ctx, server_fd);
  }

  // Verify the server’s certificate
  if (SSL_get_verify_result(ssl) != X509_V_OK) {
    fprintf(stderr, "ERROR: Certificate verification failed: %ld\n",
            SSL_get_verify_result(ssl));
    badConnection(ssl, ctx, server_fd);
  }

  printf("Connected with %s encryption\n", SSL_get_cipher(ssl));
  showingCertifications(ssl);

  // Grab creds from user
  char username[MAX_STRING_LENGTH] = {0};
  char password[MAX_STRING_LENGTH] = {0};
  printf("Username: ");
  fgetsTrim(username, sizeof(username), stdin);
  printf("Password: ");
  fgetsTrim(password, sizeof(password), stdin);

  // Send a JSON‐like request
  char buffToServer[1024];
  int len = snprintf(buffToServer, sizeof(buffToServer),
          "{ \"username\": \"%s\", \"password\": \"%s\" }",
          username, password);
  SSL_write(ssl, buffToServer, len);

  // Read server response
  char buffFromServer[1024] = {0};
  int sizeOfMessage = SSL_read(ssl, buffFromServer, sizeof(buffFromServer) - 1);
  if (sizeOfMessage > 0) {
    buffFromServer[sizeOfMessage] = '\0';
    printf("Received: %s\n", buffFromServer);
  }

  // Gracefully close the connection stream
  SSL_shutdown(ssl);
}

// Other functions as needed
/*
* An extension of fgets. Ensures the last character in the array after it calls fgets is '\0' not '\n'.
* Parameters: The same as fgets. The address to read the string entered into, the max length of the string,
* and the file stream to read in from.
* Return: None
*/
void fgetsTrim(char* string, int length, FILE* stream) {

  //use fgets
  if (!fgets(string, length, stream)) return;

  //make sure the terminating character is the null character and not the newline character
  size_t len = strlen(string);
  if (len > 0 && string[len-1] == '\n') {
      string[len-1] = '\0';
  }
}
