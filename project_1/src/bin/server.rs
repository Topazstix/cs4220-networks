use std::{
    env,
    path::PathBuf,
    sync::Arc, 
    pin::Pin
};
use tokio::{
    net::TcpListener,
    io::{AsyncReadExt, AsyncWriteExt},
};
use tokio_openssl::SslStream;
use openssl::{
    ssl::{SslMethod, SslAcceptor, SslFiletype, SslVerifyMode, Ssl, SslAcceptorBuilder},
    error::ErrorStack,
};
use serde::{Deserialize, Serialize};
use anyhow::{Result, bail};

/// serde serialization structure for username / password parameters from client
/// 
/// the *correct* client communication params should be:
/// 
/// ```json
/// {
///     "username" : "admin",
///     "password" : "password"
/// }
/// ```
#[derive(Debug, Deserialize)]
struct Credentials {
    username: String,
    password: String,
}

/// serde serialization structure for authentication response messaging to client
/// 
/// generically this struct stores whether an authentication was successful and a given error message or preconfigured server response message
#[derive(Serialize)]
struct AuthResponse {
    success: bool,
    message: String,
}

#[tokio::main]
async fn main() -> Result<()> {
    
    // Grab the root certs directory from argv
    let cert_dir = env::args().nth(1)
        .map(PathBuf::from)
        .ok_or_else(|| anyhow::anyhow!(
            "Please provide a path to the folder containing server.key, server.crt, ca.pem
            cargo run -- /path/to/certs/
            OR
            ./server /path/to/certs/"
        ))?;

    // Build each file path
    let key_path  = cert_dir.join("server.key");
    let crt_path  = cert_dir.join("server.crt");
    let ca_path   = cert_dir.join("ca.pem");

    // If one or more files do not exist in the provided dir, exit program
    if !key_path.exists() || !crt_path.exists() || !ca_path.exists() {
        bail!("One or more cert files not found in `{}`", cert_dir.display());
    }

    // Build the TLS acceptor mechanism for 
    let mut builder = SslAcceptor::mozilla_modern(SslMethod::tls_server())?;
    builder.set_private_key_file(&key_path, SslFiletype::PEM)?;
    builder.set_certificate_chain_file(&crt_path)?;
    builder.set_ca_file(&ca_path)?;
    // Dont force the client to require verification
    builder.set_verify(SslVerifyMode::NONE);

    // Initialize the SSL Acceptor for validating/generating
    // tls streams
    let acceptor = Arc::new(builder.build());

    // Bind TCP socket on localhost
    let listener = TcpListener::bind("127.0.0.1:8443").await?;
    println!("Listening on 127.0.0.1:8443 …");

    // Main brain for handling all communication streams with clients
    loop {
        
        // Set stream and socket objects w/ SSL acceptance mechanism
        let (stream, peer) = listener.accept().await?;

        // Borrow the acceptor
        // borrowing required otherwise we lose ownership 
        // when a new client requests connection
        let acceptor = acceptor.clone();

        // Configure async tokio spawn for handling client connections
        tokio::spawn(async move {

            // Wrap ssl stream and set handshake
            let ssl = match Ssl::new(acceptor.context()) {
                Ok(s) => s,
                Err(e) => { eprintln!("SSL initialization failed: {e}"); return; }
            };

            let mut tls: SslStream<tokio::net::TcpStream> =
                // Verify ssl connection succeeded in the tcp/socket stream
                match SslStream::new(ssl, stream) {
                    Ok(s) => s,
                    Err(e) => { eprintln!("Failed to create SslStream: {e}"); return; }
                };

            // Verify that the handshake succeeds
            if let Err(err) = Pin::new(&mut tls).accept().await {
                eprintln!("Handshake error from {peer}: {err}");
                return;
            }

            // Success!
            println!("TLS handshake success from {peer}");

            // Read up to 1024 bytes from client
            let mut buf = vec![0u8; 1024];
            let n = match tls.read(&mut buf).await {
                Ok(n) if n > 0 => n,
                Ok(_) => { eprintln!("No data from {peer}"); return; }
                Err(e) => { eprintln!("Read error from {peer}: {e}"); return; }
            };
            buf.truncate(n);

            // Parse incoming JSON credentials  
            let creds: Credentials = match serde_json::from_slice(&buf) {
                Ok(c) => c,
                Err(e) => {
                    eprintln!("JSON parse error from {peer}: {e}");
                    return;
                }
            };

            // Display what the user sent over wire
            println!("Client credentials: {:?}", creds);

            // Build JSON-ACK structure to send to client validating successful/invalid
            // credentials
            let resp = if creds.username == "admin" && creds.password == "password" {
                AuthResponse { success: true, message: "Welcome!".into() }
            } else {
                AuthResponse { success: false, message: "Invalid credentials".into() }
            };

            // Wrap everything for transfer back to client
            let resp_bytes = serde_json::to_vec(&resp).unwrap();

            // Send response to client
            if let Err(e) = tls.write_all(&resp_bytes).await {
                eprintln!("Write error to {peer}: {e}");
            }

            // Terminate client connection for clean exit
            let _ = tls.shutdown().await;
        });
    }
}
