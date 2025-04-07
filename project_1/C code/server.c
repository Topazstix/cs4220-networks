/* This program is designed to represent a server in C. It is setup 
to communicate securely via open SSL. The client will encrypt and send a message 
after validating certificates with the given server. */


// Included
#include <stdio.h>
#include <errno.h>
#include <malloc.h>
#include <string.h>

// Global constant definitions
#define FAIL -1
#include <stdio.h>
#include <errno.h>
#include <malloc.h>
#include <string.h>


/* Main function to simulate the client sending a request to a server.
Arguments: Needs the portnumber
Returns: idk yet nothing important probably just an exit code.
*/
int main(int argc, char * argv[]){

    /* Error checking */ 
    // Check if user is root (only matters if port is <1000)


    /* Check if right number of arguments */
    if (argc != 2) {

        printf ("Error: Wrong number of arguments\nUsage: %s <port number>\n", argv[0]);
        exit(0);
    }

    /* Definitions */
    char *portnum = argv[1];


    /* Start SSL/Socket stuff*/
    // Handshake phase: client sends request and server sends certificate

    // Client validates certificate 

    // Client creates private session key and sends packet encrypted with server's pubblic key

    // Server now has access to private session key

    // Encrypted communication can start between client and server


    /* Listening */
    // Infinite listening loop 
    while (1) {

        /* Recieving */


        /* Validating */


        /* Transfer Message to User */


    }


    /* End */
    // Manage memory (probably not needed)
    free(portnum);
    // Close socket and connections
    // close(server);

}



// Other functions as needed


