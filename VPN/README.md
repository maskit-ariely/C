
# VPN in C

This project is a single client VPN implementation in C that allows secure communication using SSL/TLS encryption.

## Features

- Secure communication between a client and a server using SSL/TLS encryption
- Uses TUN/TAP devices for creating virtual network interfaces
- Provides automatic routing and network configuration
## Requirements

- Two Linux-based systems 
- OpenSSL library (`sudo apt install openssl libssl-dev`)
- net-tools library (`sudo apt install net-tools`)
- Root access (some operations require superuser privileges)
- GCC compiler

## Before Getting Started
- Make sure you have OpenSSL and net-tools libraries installed
- Create a self-signed certificate root CA using [this article](https://www.linkedin.com/pulse/how-create-your-own-self-signed-root-certificate-shankar-gomare/)
- Both client and server have dedicated configuration files, ensure you fill in the parameters correctly
## Compilation and Usage

1. Clone or download the repository to your local machine.
2. Open a terminal and navigate to the directory containing the downloaded files.
3. Compile the code (server and client):
   ```bash
   gcc server.c -o server -lssl -lcrypto
   ```
   ```bash
   gcc client.c -o client -lssl -lcrypto
   ```
4. Execute the programs with the following commands:
   ```bash
   sudo ./server
   ```
   ```bash
   sudo ./client
   ```
## Demo

Network Configuration:
![vpn_network_diagram](VPN_network_diagram.png)


[![VPN demo](https://res.cloudinary.com/marcomontalbano/image/upload/v1691938076/video_to_markdown/images/youtube--qkAdMvXr7jc-c05b58ac6eb4c4700831b2b3070cd403.jpg)](https://www.youtube.com/watch?v=qkAdMvXr7jc&ab_channel=SOMEONE "VPN demo")
