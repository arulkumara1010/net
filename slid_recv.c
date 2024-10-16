#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <asm-generic/socket.h>

#define PORT 8080
#define BUFFER_SIZE 1024
#define MAX_SEQ_NO 7
#define RWS ((MAX_SEQ_NO + 1) / 2)

void send_ack(int sock, int ack)
{
    char buffer[BUFFER_SIZE];
    snprintf(buffer, BUFFER_SIZE, "%d", ack);
    printf("Receiver: Sending ACK for frame %d\n", ack);
    send(sock, buffer, strlen(buffer), 0);
}

void send_nack(int sock, int expected)
{
    char buffer[BUFFER_SIZE];
    snprintf(buffer, BUFFER_SIZE, "%d", -1);
    printf("Receiver: Sending NACK, expected frame %d\n", expected);
    send(sock, buffer, strlen(buffer), 0);
}
int receive_frame(int sock)
{
    char buffer[BUFFER_SIZE];
    memset(buffer, 0, BUFFER_SIZE);
    read(sock, buffer, BUFFER_SIZE);
    int frame = atoi(buffer);
    return frame;
}

int main()
{
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }

    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)))
    {
        perror("setsockopt failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("Bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 3) < 0)
    {
        perror("Listen failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("Waiting for a connection on port %d...\n", PORT);

    if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0)
    {
        perror("Accept failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("Connected to client.\n");

    int LFR = -1;
    int LAF = 0;
    int N = 0;

    while (1)
    {
        int frame = receive_frame(new_socket);

        if (frame == N)
        {
            printf("Receiver: Frame %d received in order\n", frame);
            send_ack(new_socket, frame);
            LFR = frame;
            N = (N + 1) % (MAX_SEQ_NO + 1);
        }
        else
        {
            printf("Receiver: Frame %d out of order, sending NACK\n", frame);
            send_nack(new_socket, N);
        }
    }

    close(new_socket);
    close(server_fd);
    return 0;
}
