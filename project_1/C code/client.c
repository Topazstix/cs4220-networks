/* This program is designed to represent a client server in C. It is setup to communicate securely via open SSL. The client will encrypt and send a message 
after validating certificates with the given server. */


// Included
#include <stdio.h>
#include <errno.h>
#include <malloc.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <resolv.h>
#include <netdb.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

// Define
#define MAX_STRING_LENGTH 80


// Functions for the connection
/* Function to open the connection for the client.
Arguments: the hostname passed as a constant string and the port number as an integer
Returns: the socket file descriptor
*/
int OpenConnection(const char *hostname, int port){

    // Get and verify the hostname
    struct hostent *host;
    host = gethostbyname(hostname);
    if (host == NULL){
        perror("ERROR: HOSTNAME IS INVALID");
        abort();
    }

    // Create the socket
    int socketID = socket(AF_INET, SOCK_STREAM, 0);

    // Check socket status
    if (socketID == -1){
        perror("ERROR: SOCKET WAS NOT CREATED");
        abort();
    }

    // Prepare for connection
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));     // Removes padding from struct
    addr.sin_family = PF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = *(long*)(host->h_addr);
    

    // Connect and check connection
    if (connect(socketID, (struct sockaddr *)&sockaddr_in, sizeof(sockaddr_in)) == -1){
        perror("ERROR: CANNOT CONNECT TO SERVER");
        abort();
    }

    // Return socket file descriptor
    return socketID;

}

/* Function to show the certificate of the server
Arguments: An SSL pointer that represents an SSL connection
Returns: Nothing
*/
void showingCertifications (SSL *ssl){

    // Declarations
    char *subject;
    char *issuer;

    // Get Server's Cert
    X509 *cert = SSL_get_peer_certificate(ssl);

    // Validate and display
    if (cert == NULL){
        printf("There are no configured client certificates.\n");
    }

    else{
        printf("Server certificates:\n");
        subject = X509_NAME_oneline(X509_get_subject_name(cert), 0, 0);
        printf("Certificate subject: %s\n", subject);

        issuer = X509_NAME_oneline(X509_get_issuer_name(cert), 0, 0);
        printf("Issued by: %s\n", issuer);

    }

    // Free memory
    free(subject);
    free(issuer);
    X509_free(cert);
}


/* Function to setup SSL_CTX 
Arguments: None
Returns: a pointer to an SSL_CTX structure containing information about the connection
*/
SSL_CTX* setupCTX(){

    OpenSSL_add_all_algorithms();   // Gather all of the cryptos
    SSL_load_error_strings();       // Load error strings for human readable errors
    SSL_METHOD *method = TLSv1_2_client_method();
    SSL_CTX *ctx = SSL_CTX_new(method);
    
    // Check connection
    if (ctx == NULL){
        ERR_print_errors_fp(stderr);
        abort();
    }
    
    return ctx;
}


/* Main function to simulate the client sending a request to a server.
Arguments: Gets the number of arguments as the first argument and the rest of the arguments as an array of strings.
Returns: idk yet nothing important probably just an exit code.
*/
int main(int argc, char *argv[]){
    
    
    // Initialize variables
    char message[1024];


    // Check to see if the program was called with the right number of arguments.
    if (argc != 3){
        printf("Improper usage. Expected compiledClient.exe <hostname> <portnum>");
        exit(0);
    }

    // Create socket/SSL connection thing


    // Validate certificate with SSL


    // Try to connect

    //if SSL validation failed 
        // then exit
    //else 
        // Declare the spots for the password and username
        char username[16] = {0};
        char password[16] = {0};

    
        // Print prompts after validation
        // User needs their password and username
        printf("Please enter your username: \n");
        fgetsTrim(username, MAX_STRING_LENGTH, stdin);
        printf("Please enter your password: \n");
        fgetsTrim(password, MAX_STRING_LENGTH, stdin);

        // Send the user's credentials along with their request to the server


        // Recieve the message
        printf("Recieved a message: \"%s\"\n", message);


    // Close the connection

    return 0;

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
	fgets(string, length, stdin);

	//make sure the terminating character is the null character and not the newline character
	if (string[strlen(string) - 1] == '\n') {
		string[strlen(string) - 1] = '\0';
	}
}

