/* This program is designed to represent a client server in C. It is setup to communicate securely via open SSL. The client will encrypt and send a message 
after validating certificates with the given server. */


// Included
#include <stdio.h>
#include <errno.h>
#include <malloc.h>
#include <string.h>




/* Main function to simulate the client sending a request to a server.
Arguments: Gets the number of arguments as the first argument and the rest of the arguments as an array of strings.
Returns: idk yet nothing important probably just an exit code.
*/
int main(int argc, char *argv[]){
    
    
    // Initialize variables
    char buff[1024];


    // Check to see if the program was called with the right number of arguments.
    if (argc != 3){
        printf("Improper usage. Expected compiledClient.exe <hostname> <portnum>");
        exit(0);
    }

    // Find target


    // Try to connect

    

    // Inside the loop if the SSL_connect is true
    // Declare the spots for the password and username
    char username[16] = {0};
    char password[16] = {0};

    
    // Print prompts after validation
    printf("Please enter your username: \n");
    // scanf and validate for user 
    printf("Please enter your username: \n");

}


// Other functions as needed


