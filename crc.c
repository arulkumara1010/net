#include <stdio.h>
#include <string.h>

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

    printf("Original Message (M(x)): %s\n", message);
    printf("Divisor Polynomial (C(x)): %s\n", divisor);
    printf("Transmitted Message (M(x) + R(x)): %s\n", transmittedMessage);
    printf("Remainder (R(x)): %s\n", remainder);

    return 0;
}
