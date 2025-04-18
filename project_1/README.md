# Project One: Client/Server

Goal is to create a client/server program which utilizes openssl to generate a TLS certificate and encrypt/authenticate communications between a client & a server. Example program code has been included in the [examples](./examples/README.md) folder

## Demo! :)

[![asciicast](https://asciinema.org/a/wImOlzFrFosmUwt2g7sKnU79e.svg)](https://asciinema.org/a/wImOlzFrFosmUwt2g7sKnU79e)

## Server

Our server is written and configured in rust. The server is designed to load a pre-generated (user generated) certificate for TLS authentication and communication. Once certificates are loaded via openssl, a socket stream is initiated and then configured to utilize SSL streams for client/server communications.

## Client

Our client is written and designed in C, with a few additional security mechanisms implemented to handle safely deallocating memory and closing socket connections. The client preloads the server's certificate, loads this into openssl, and uses this as the agreed upon means for generating TLS traffic with the server. Additionally, the client structures its message for the server in a JSON-*like* struct, which the server deserializes as native JSON for verification.

## Initialization

The following commands must be run in order to generate certificates for the programs to utilize, preferably stored in an easy to reference `certs` folder (or wherever you prefer)

### Certificate Generation

> NOTE: These instructions assume linux operating system, adjust commands accordingly per OS

```bash
## Make certs folder and install prereqs for cert gen
mkdir certs && cd certs
sudo apt update
sudo apt install -y openssl libssl-dev build-essential

## Create a root CA key & self‑signed cert
openssl genrsa -out ca.key 4096
openssl req -x509 -new -nodes -key ca.key \
    -days 30 -subj "/CN=Project 1 CA" \
    -out ca.pem

## Create server key & CSR
openssl genrsa -out server.key 4096
openssl req -new -key server.key \
    -subj "/CN=localhost" \
    -out server.csr

## Sign server CSR with your CA
openssl x509 -req -in server.csr -CA ca.pem -CAkey ca.key \
    -CAcreateserial -out server.crt -days 30 -sha256

## (Optional) inspect certs:
openssl x509 -noout -text -in server.crt
```

### Compile Server

> NOTE: Requires rust installation, preferably install via [rustup](https://rustup.rs/)

```bash
cargo build -p server_client ## In root dir

## Debugging run
cargo run -p server_client --bin server -- project_1/certs
```

### Compile Client

> NOTE: Requires `build-essentials`, `openssl`, `libssl-dev` pkgs in debian like or `msvc` / windows equivalent of `gcc` AND a valid openssl installation on windows.

```bash
gcc client.c -o client $(pkg-config --cflags --libs openssl)
```