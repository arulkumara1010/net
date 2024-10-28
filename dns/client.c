#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define ROOT_PORT 8080
#define BUF_SIZE 512

int main()
{
    int sockfd;
    char buffer[BUF_SIZE];
    struct sockaddr_in servaddr;

    // Create UDP socket
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&servaddr, 0, sizeof(servaddr));

    // Root server information
    servaddr.sin_family = AF_INET; // IPv4
    servaddr.sin_port = htons(ROOT_PORT);
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1"); // Root server IP

    char domain[50];

    while (1)
    {
        // Enter domain name
        printf("Enter URL to be resolved: ");
        scanf("%s", domain);

        // Send the domain name to the root server
        sendto(sockfd, domain, strlen(domain), 0, (const struct sockaddr *)&servaddr, sizeof(servaddr));
        printf("Query sent for domain: %s\n", domain);

        // Receive TLD server address
        int n = recvfrom(sockfd, buffer, BUF_SIZE, 0, NULL, NULL);
        buffer[n] = '\0'; // Null-terminate the received string
        printf("TLD server address received: %s\n", buffer);

        // Now query the TLD server
        char *tld_server = buffer;
        memset(&servaddr, 0, sizeof(servaddr));
        servaddr.sin_family = AF_INET;                     // IPv4
        servaddr.sin_port = htons(8081);                   // Assuming the TLD server for .com is on port 8081
        servaddr.sin_addr.s_addr = inet_addr("127.0.0.1"); // Replace with TLD server IP

        // Send the domain name to the TLD server
        sendto(sockfd, domain, strlen(domain), 0, (const struct sockaddr *)&servaddr, sizeof(servaddr));
        printf("DNS query sent to TLD server: %s\n", domain);

        // Receive the IP address from the TLD server
        n = recvfrom(sockfd, buffer, BUF_SIZE, 0, NULL, NULL);
        buffer[n] = '\0'; // Null-terminate the received string
        printf("IP address received: %s\n", buffer);
    }

    close(sockfd);
    return 0;
}
