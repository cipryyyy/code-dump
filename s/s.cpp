#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#define PKG_SIZE    4
#define ITERATIONS  1000

int main(int argc, char* argv[])
{
    // Create socket
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);

    // Define address
    sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(9876);
    server_address.sin_addr.s_addr = INADDR_ANY;

    // Binding
    bind(server_socket, (struct sockaddr*)&server_address, sizeof(server_address));

    // Listen
    listen(server_socket, 5);

    // Accept
    int client_socket = accept(server_socket, nullptr, nullptr);

    // Receive data
    char cmd;
    char buffer[PKG_SIZE] = {0};
    for (int i = 0; i < ITERATIONS; i++)
    {
        recv(client_socket, &buffer, sizeof(char), 0);

        switch (cmd)
        {
            case 't':
                recv(client_socket, buffer, sizeof(buffer), 0);
                break;
            case 'r':
                send(client_socket, buffer, sizeof(buffer), 0);
                break;
            default:
                break;
        }
    }

    close(client_socket);
    
    return 0;
}