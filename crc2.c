#include <stdio.h>
#include <string.h>

// Function to perform XOR operation
void xorOperation(char *dividend, const char *divisor, int len)
{
    for (int i = 0; i < len; i++)
    {
        dividend[i] = (dividend[i] == divisor[i]) ? '0' : '1';
    }
}

// Function to perform CRC division
void crcDivision(char *dividend, const char *divisor, int dividendLen, int divisorLen)
{
    int currentPos = 0;

    printf("Division process:\n");
    while (currentPos <= dividendLen - divisorLen)
    {
        // Only proceed if the current position has '1'
        if (dividend[currentPos] == '1')
        {
            printf("Step %d: ", currentPos);
            printf("Dividend before XOR: %s\n", dividend);
            xorOperation(&dividend[currentPos], divisor, divisorLen);
            printf("Dividend after XOR:   %s\n", dividend);
        }
        currentPos++;
    }
    printf("Final remainder:      %s\n", &dividend[dividendLen - (divisorLen - 1)]);
}

// Function to send the transmitted message
void sender(const char *message, const char *divisor)
{
    int messageLen = strlen(message);
    int divisorLen = strlen(divisor);
    int k = divisorLen - 1;

    // Create extended message
    char extendedMessage[messageLen + k + 1];
    strcpy(extendedMessage, message);
    for (int i = 0; i < k; i++)
    {
        extendedMessage[messageLen + i] = '0';
    }
    extendedMessage[messageLen + k] = '\0';

    // Perform CRC division to get the remainder
    crcDivision(extendedMessage, divisor, messageLen + k, divisorLen);

    // Extract the remainder
    char remainder[k + 1];
    strncpy(remainder, &extendedMessage[messageLen], k);
    remainder[k] = '\0';

    // Create the transmitted message (original + remainder)
    char transmittedMessage[messageLen + k + 1];
    strcpy(transmittedMessage, message);
    strcat(transmittedMessage, remainder);

    // Print original and transmitted messages
    printf("Original Message (M(x)): %s\n", message);
    printf("Divisor Polynomial (C(x)): %s\n", divisor);
    printf("Transmitted Message (M(x) + R(x)): %s\n", transmittedMessage);
}

// Function to receive the message and check for errors
void receiver(const char *transmittedMessage, const char *divisor)
{
    int messageLen = strlen(transmittedMessage);
    int divisorLen = strlen(divisor);
    int k = divisorLen - 1;

    // Create a copy of the transmitted message for CRC division
    char receivedMessage[messageLen + 1];
    strcpy(receivedMessage, transmittedMessage);

    // Perform CRC division to check for remainder
    crcDivision(receivedMessage, divisor, messageLen, divisorLen);

    // Check for remainder (should be all zeros if no error)
    int errorDetected = 0;
    for (int i = messageLen; i < messageLen + k; i++)
    {
        if (receivedMessage[i] == '1')
        {
            errorDetected = 1; // Error detected
            break;
        }
    }

    printf("Received Message: %s\n", transmittedMessage);
    if (errorDetected)
    {
        printf("Error detected in the received message.\n");
    }
    else
    {
        printf("No error detected in the received message.\n");
    }
}

int main()
{
    char message[100], divisor[100];

    // Input the original message and divisor
    printf("Enter the message (binary): ");
    scanf("%s", message);
    printf("Enter the divisor polynomial (binary): ");
    scanf("%s", divisor);

    // Sender function to send the transmitted message
    sender(message, divisor);

    // For the purpose of this example, let's simulate receiving the transmitted message
    // In a real application, this would come from a communication channel
    char transmittedMessage[100];
    strcpy(transmittedMessage, message); // Start with the original message
    strcat(transmittedMessage, "000");   // Simulate appending zeros for CRC check
    strcat(transmittedMessage, "011");   // Simulated remainder for this example (can modify for testing)

    // Receiver function to check the received message
    receiver(transmittedMessage, divisor);

    return 0;
}
