/* This program is designed to represent a client server in C. It is setup to communicate securely via open SSL. The client will encrypt and send a message 
after validating certificates with the given server. */


// Included
#include <stdio.h>
#include <errno.h>
#include <malloc.h>
#include <string.h>


// Define
#define MAX_STRING_LENGTH 80

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

