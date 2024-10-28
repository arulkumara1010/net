#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8081
#define BUF_SIZE 512

// Structure for DNS records
struct dns_record
{
    char domain[50];
    char ip[16];
};

// DNS records for the TLD server
struct dns_record records[] = {
    {"www.google.com", "8.8.8.8"},
    {"www.facebook.com", "157.240.22.35"},
};

int find_ip(char *domain, char *ip)
{
    for (int i = 0; i < sizeof(records) / sizeof(records[0]); i++)
    {
        if (strcmp(records[i].domain, domain) == 0)
        {
            strcpy(ip, records[i].ip);
            return 1; // Found the domain
        }
    }
    return 0; // Domain not found
}

int main()
{
    int sockfd;
    char buffer[BUF_SIZE];
    struct sockaddr_in servaddr, cliaddr;

    // Create UDP socket
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&servaddr, 0, sizeof(servaddr));
    memset(&cliaddr, 0, sizeof(cliaddr));

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
    char ip[16];

    len = sizeof(cliaddr); // Length of the client address

    while (1)
    {
        // Receive domain name from root server
        n = recvfrom(sockfd, buffer, BUF_SIZE, 0, (struct sockaddr *)&cliaddr, &len);
        buffer[n] = '\0'; // Null-terminate the received string
        printf("Received domain from root server: %s\n", buffer);

        // Find the corresponding IP for the domain
        if (find_ip(buffer, ip))
        {
            sendto(sockfd, ip, strlen(ip), 0, (struct sockaddr *)&cliaddr, len);
        }
        else
        {
            char *not_found = "Domain not found";
            sendto(sockfd, not_found, strlen(not_found), 0, (struct sockaddr *)&cliaddr, len);
        }
    }

    close(sockfd);
    return 0;
}
