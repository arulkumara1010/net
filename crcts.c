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

    int sock;
    struct sockaddr_in server_addr;

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        printf("Socket creation failed\n");
        return 1;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8080);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        printf("Connection to server failed\n");
        return 1;
    }
    send(sock, transmittedMessage, strlen(transmittedMessage), 0);
    printf("Message sent to receiver.\n");

    close(sock);
    return 0;
}
