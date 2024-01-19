# Python Socket Server Usage Instructions

This repository contains a simple Python server that can be used to receive data via a socket. Follow the instructions below to set up and use the server.

## Prerequisites

- Python 3.x installed on your system.

## Configuration


1. Clone the repository to your system:

2. Navigate to the project directory:

3. Open the `main.py` file in a text editor of your choice.

4. Configure the server parameters at the beginning of the `main.py` file:

- `struct_format`: Set the format of the message structure according to the expected data.
- `server_ip`: Set the IP address at which the server should listen for connections.
- `server_port`: Set the port on which the server should listen for connections.

## Running the Server

Run the Python server with the following commands:

```sudo python3 main.py
The server will start listening for connections on the specified IP address and port.


You will need to create a client that sends data in the format specified by struct_format to the configured IP address and port of the server.
