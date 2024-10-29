#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

#define PORT 8080
#define BUFFER_SIZE 1024
#define SEND_KEYWORD "SEND"

int sockfd;
struct sockaddr_in server_addr, client_addr;
socklen_t addr_len = sizeof(client_addr);

void *receive_messages(void *arg)
{
    char buffer[BUFFER_SIZE];
    while (1)
    {
        int n = recvfrom(sockfd, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&client_addr, &addr_len);
        buffer[n] = '\0';
        printf("Client: %s\n", buffer);
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
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // Bind socket to the server address
    if (bind(sockfd, (const struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("Bind failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    printf("UDP server is up and listening on port %d\n", PORT);

    // Create a thread for receiving messages
    pthread_t recv_thread;
    pthread_create(&recv_thread, NULL, receive_messages, NULL);

    // Send messages
    while (1)
    {
        printf("Enter message to send to Client (end message with 'SEND'): ");
        fgets(buffer, BUFFER_SIZE, stdin);
        buffer[strcspn(buffer, "\n")] = '\0'; // Remove newline

        // Check if the message ends with "SEND"
        if (strstr(buffer, SEND_KEYWORD) != NULL && strstr(buffer, SEND_KEYWORD) == buffer + strlen(buffer) - strlen(SEND_KEYWORD))
        {
            // Remove "SEND" from the end
            buffer[strlen(buffer) - strlen(SEND_KEYWORD)] = '\0';
            sendto(sockfd, buffer, strlen(buffer), 0, (struct sockaddr *)&client_addr, addr_len);
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
