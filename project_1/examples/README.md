# Example Client/Server C Program

Example program code for a client/server application were grabbed from an [article](https://aticleworld.com/ssl-server-client-using-openssl-in-c/) which covers in detail how to design and deploy this stack utilizing `openssl` for tls communication between each component.

> **NOTE:** Rather than retain the examples default `XML` structured messages & responses, translated to a pseudo `JSON` format for simplification.

## Instructions

For detailed information on openssl & client/server communications, please reference the [article](https://aticleworld.com/ssl-server-client-using-openssl-in-c/) utilized here in these examples.

1. Verify that openssl is installed on platform
   1. for ubuntu:
      1. `sudo apt install -y openssl libssl-dev build-essential`
2. Generate an openssl certificate (note: article requires `1024` bit key, this has been adjusted to `2048` for modern standard. `4096` bit would be best, but hey its an example)
   1. `openssl req -x509 -nodes -days 365 -newkey rsa:1024 -keyout mycert.pem -out mycert.pem`
   2. Mash enter through all requests for information on the certificate owner; this information is not required for an example
3. Compile each program into a binary
   1. `gcc -Wall -o client  example_client.c -L/usr/lib -lssl -lcrypto`
   2. `gcc -Wall -o server example_server.c -L/usr/lib -lssl -lcrypto`
4. Start the server on port `8080` (any port above `1000` should be acceptable)
   1. `./server 8080`
5. Execute the client and input required information
   1. `./client 127.0.0.1 8080`
   2. The "correct" inputs, according to the code and the original developer **amlendra** are:
      1. User: `article`
      2. Password: `123`
   3. Other inputs will force the server to send a response "Invalid Message" whereas the correct input will return information about the original developer in pseudo-json format.
