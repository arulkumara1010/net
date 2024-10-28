#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUF_SIZE 512
#define TLD_COM_PORT 8081 // Port for .com TLD server
#define TLD_ORG_PORT 8082 // Port for .org TLD server

// Function to extract the TLD from the domain name
char *get_tld(char *domain)
{
    char *dot = strrchr(domain, '.');
    return (dot != NULL) ? dot + 1 : NULL; // Return the TLD (e.g., com, org)
}

int main()
{
    int sockfd;
    char buffer[BUF_SIZE];
    struct sockaddr_in servaddr, cliaddr, tld_addr;

    // Create UDP socket
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&servaddr, 0, sizeof(servaddr));
    memset(&cliaddr, 0, sizeof(cliaddr));
    memset(&tld_addr, 0, sizeof(tld_addr));

    // Server information
    servaddr.sin_family = AF_INET; // IPv4
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(PORT);

    // Bind the socket with the server address
    if (bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
    {
        perror("Bind failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    int len, n;
    len = sizeof(cliaddr); // Length of the client address

    while (1)
    {
        // Receive domain name from client
        n = recvfrom(sockfd, buffer, BUF_SIZE, 0, (struct sockaddr *)&cliaddr, &len);
        buffer[n] = '\0'; // Null-terminate the received string
        printf("Received domain from client: %s\n", buffer);

        // Determine the TLD
        char *tld = get_tld(buffer);
        if (tld == NULL)
        {
            char *not_found = "Invalid domain";
            sendto(sockfd, not_found, strlen(not_found), 0, (struct sockaddr *)&cliaddr, len);
            continue;
        }

        // Set TLD server address based on the TLD
        if (strcmp(tld, "com") == 0)
        {
            tld_addr.sin_port = htons(TLD_COM_PORT);
        }
        else if (strcmp(tld, "org") == 0)
        {
            tld_addr.sin_port = htons(TLD_ORG_PORT);
        }
        else
        {
            char *not_found = "Unsupported TLD";
            sendto(sockfd, not_found, strlen(not_found), 0, (struct sockaddr *)&cliaddr, len);
            continue;
        }

        // Forward the request to the appropriate TLD server
        tld_addr.sin_family = AF_INET;
        tld_addr.sin_addr.s_addr = inet_addr("127.0.0.1"); // Assuming TLD servers run locally

        // Send the request to the TLD server
        sendto(sockfd, buffer, strlen(buffer), 0, (const struct sockaddr *)&tld_addr, sizeof(tld_addr));

        // Receive the IP address from the TLD server
        n = recvfrom(sockfd, buffer, BUF_SIZE, 0, NULL, NULL);
        buffer[n] = '\0'; // Null-terminate the received string

        // Send the IP address back to the client
        sendto(sockfd, buffer, strlen(buffer), 0, (struct sockaddr *)&cliaddr, len);
    }

    close(sockfd);
    return 0;
}
