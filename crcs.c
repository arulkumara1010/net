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
    char message[100], divisor[100];

    printf("Enter the message (binary): ");
    scanf("%s", message);
    printf("Enter the divisor polynomial (binary): ");
    scanf("%s", divisor);

    int messageLen = strlen(message);
    int divisorLen = strlen(divisor);
    int k = divisorLen - 1;

    char extendedMessage[messageLen + k + 1];
    strcpy(extendedMessage, message);
    for (int i = 0; i < k; i++)
    {
        extendedMessage[messageLen + i] = '0';
    }
    extendedMessage[messageLen + k] = '\0';

    crcDivision(extendedMessage, divisor, messageLen + k, divisorLen);

    char remainder[k + 1];
    strncpy(remainder, &extendedMessage[messageLen], k);
    remainder[k] = '\0';

    char transmittedMessage[messageLen + k + 1];
    strcpy(transmittedMessage, message);
    strcat(transmittedMessage, remainder);

    printf("Transmitted Message: %s\n", transmittedMessage);

    // UDP code to send the message
    int sock;
    struct sockaddr_in receiver_addr;

    // Create socket
    if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
    {
        printf("Socket creation failed\n");
        return 1;
    }

    // Setup receiver address
    receiver_addr.sin_family = AF_INET;
    receiver_addr.sin_port = htons(8080);                   // receiver port
    receiver_addr.sin_addr.s_addr = inet_addr("127.0.0.1"); // receiver IP

    // Send the transmitted message
    sendto(sock, transmittedMessage, strlen(transmittedMessage), 0, (struct sockaddr *)&receiver_addr, sizeof(receiver_addr));

    printf("Message sent to receiver.\n");

    close(sock);
    return 0;
}
