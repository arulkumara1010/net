#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>

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
    }
}

int main()
{
    char divisor[100], receivedMessage[200];

    printf("Enter the divisor polynomial (binary): ");
    scanf("%s", divisor);

    int divisorLen = strlen(divisor);

    // UDP code to receive the message
    int sock;
    struct sockaddr_in receiver_addr, sender_addr;
    socklen_t addr_len = sizeof(sender_addr);
    char buffer[200];

    // Create socket
    if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
    {
        printf("Socket creation failed\n");
        return 1;
    }

    // Setup receiver address
    receiver_addr.sin_family = AF_INET;
    receiver_addr.sin_port = htons(8080); // receiver port
    receiver_addr.sin_addr.s_addr = INADDR_ANY;

    // Bind socket
    if (bind(sock, (struct sockaddr *)&receiver_addr, sizeof(receiver_addr)) == -1)
    {
        printf("Bind failed\n");
        return 1;
    }

    printf("Waiting for message...\n");

    // Receive message from sender
    int len = recvfrom(sock, buffer, 200, 0, (struct sockaddr *)&sender_addr, &addr_len);
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

    close(sock);
    return 0;
}
