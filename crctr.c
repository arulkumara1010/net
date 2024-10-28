#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

void xorOperation(char *dividend, const char *divisor, int len)
{
    for (int i = 0; i < len; i++)
    {
        dividend[i] = (dividend[i] == divisor[i]) ? '0' : '1';
    }
}

void crcDivision(char *dividend, const char *divisor, int dividendLen, int divisorLen)
{
    int currentPos = 0;

    while (currentPos <= dividendLen - divisorLen)
    {
        if (dividend[currentPos] == '1')
        {
            xorOperation(&dividend[currentPos], divisor, divisorLen);
        }
        currentPos++;
        printf(dividend);
        printf("\n");
    }
}

int main()
{
    char divisor[100], receivedMessage[200];

    printf("Enter the divisor polynomial (binary): ");
    scanf("%s", divisor);

    int divisorLen = strlen(divisor);

    // TCP code to receive the message
    int sock, new_sock;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);
    char buffer[200];

    // Create socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        printf("Socket creation failed\n");
        return 1;
    }

    // Setup server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8080); // server port
    server_addr.sin_addr.s_addr = INADDR_ANY;

    // Bind socket
    if (bind(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
    {
        printf("Bind failed\n");
        return 1;
    }

    // Listen for incoming connections
    listen(sock, 3);

    printf("Waiting for connection...\n");

    // Accept incoming connection
    new_sock = accept(sock, (struct sockaddr *)&client_addr, &addr_len);
    if (new_sock < 0)
    {
        printf("Accept failed\n");
        return 1;
    }

    // Receive message from sender
    int len = recv(new_sock, buffer, 200, 0);
    buffer[len] = '\0'; // Null terminate the received message

    strcpy(receivedMessage, buffer);
    printf("Received Message: %s\n", receivedMessage);

    int receivedLen = strlen(receivedMessage);
    crcDivision(receivedMessage, divisor, receivedLen, divisorLen);

    // Check if remainder is zero (no error)
    int errorDetected = 0;
    for (int i = receivedLen - (divisorLen - 1); i < receivedLen; i++)
    {
        if (receivedMessage[i] != '0')
        {
            errorDetected = 1;
            break;
        }
    }

    if (errorDetected)
    {
        printf("Error detected in received message.\n");
    }
    else
    {
        printf("No error detected, message received correctly.\n");
    }

    close(new_sock);
    close(sock);
    return 0;
}
