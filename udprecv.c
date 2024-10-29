#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

#define PORT 8080
#define SERVER_IP "127.0.0.1" // Change this to the server's IP address
#define BUFFER_SIZE 1024
#define SEND_KEYWORD "SEND"

int sockfd;
struct sockaddr_in server_addr;

void *receive_messages(void *arg)
{
    char buffer[BUFFER_SIZE];
    socklen_t addr_len = sizeof(server_addr);

    while (1)
    {
        int n = recvfrom(sockfd, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&server_addr, &addr_len);
        buffer[n] = '\0';
        printf("Server: %s\n", buffer);
    }
    return NULL;
}

int main()
{
    char buffer[BUFFER_SIZE];

    // Create socket
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Configure server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr);

    // Create a thread for receiving messages
    pthread_t recv_thread;
    pthread_create(&recv_thread, NULL, receive_messages, NULL);

    // Send messages
    while (1)
    {
        printf("Enter message to send to Server (end message with 'SEND'): ");
        fgets(buffer, BUFFER_SIZE, stdin);
        buffer[strcspn(buffer, "\n")] = '\0'; // Remove newline

        // Check if the message ends with "SEND"
        if (strstr(buffer, SEND_KEYWORD) != NULL && strstr(buffer, SEND_KEYWORD) == buffer + strlen(buffer) - strlen(SEND_KEYWORD))
        {
            // Remove "SEND" from the end
            buffer[strlen(buffer) - strlen(SEND_KEYWORD)] = '\0';
            sendto(sockfd, buffer, strlen(buffer), 0, (const struct sockaddr *)&server_addr, sizeof(server_addr));
        }
        else
        {
            printf("Message should end with 'SEND' to be sent.\n");
        }
    }

    pthread_join(recv_thread, NULL);
    close(sockfd);
    return 0;
}