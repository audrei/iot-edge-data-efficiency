import struct
import socket

# Define the structure of the struct
# The format of the struct depends on the data you expect to receive.
# For example, if you are expecting four integers, use "i i i i".
struct_format = "i i f"

struct_size = struct.calcsize(struct_format)

# Set the server's IP address and port
server_ip = '192.168.1.112'
server_port = 1010

# Create the TCP socket
server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

# Bind the server socket to the specified address and port
server_address = (server_ip, server_port)
server_socket.bind(server_address)

server_socket.listen(1)

print('Server waiting for connection on IP:', server_ip, 'Port:', server_port)

while True:
    # Accept a connection
    client_socket, client_address = server_socket.accept()
    print('Client connected:', client_address)

    # Receive the data buffer
    data = client_socket.recv(1024)

    # Check if there is enough data for at least one structure
    while len(data) >= struct_size:
        # Unpack the data from the struct
        sensor_data = struct.unpack(struct_format, data[:struct_size])

        # Print the data separately
        print('deviceId:', sensor_data[0])
        print('measurementType:', sensor_data[1])
        print('value: {:.2f}'.format(sensor_data[2]))
        #print('timestamp:', sensor_data[3])  # Uncomment this line if necessary
        #print('errorCode:', sensor_data[3])  # Uncomment this line if necessary
        print('---')  # Separator between structures

        # Remove the data from the structure from the buffer
        data = data[struct_size:]

    # Close the connection with the client
    client_socket.close()
