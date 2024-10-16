#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024
#define MAX_SEQ_NO 7
#define SWS ((MAX_SEQ_NO + 1) / 2)

void send_frame(int sock, int frame)
{
    char buffer[BUFFER_SIZE];
    snprintf(buffer, BUFFER_SIZE, "%d", frame);
    printf("Sender: Sending frame %d\n", frame);
    send(sock, buffer, strlen(buffer), 0);
}

int receive_ack(int sock)
{
    char buffer[BUFFER_SIZE];
    memset(buffer, 0, BUFFER_SIZE);
    read(sock, buffer, BUFFER_SIZE);
    int ack = atoi(buffer);
    return ack;
}

int main()
{
    int sock = 0;
    struct sockaddr_in serv_addr;

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("Socket creation error");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0)
    {
        perror("Invalid address/Address not supported");
        return -1;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        perror("Connection failed");
        return -1;
    }

    int LFS = 0;
    int LAR = -1;
    int frame_count = 7;
    int msg = 0;
    int custom_order[SWS];
    int i = 0;

    printf("Enter the frame sequence in which to send (%d frames):\n", frame_count);
    for (i = 0; i < frame_count; i++)
    {
        printf("Enter frame %d: ", i + 1);
        scanf("%d", &custom_order[i]);
    }

    i = 0;

    while (frame_count > 0)
    {
        send_frame(sock, custom_order[i]);
        LFS = (custom_order[i] + 1) % (MAX_SEQ_NO + 1);
        i++;
        int ack = receive_ack(sock);
        if (ack >= 0)
        {
            LAR = ack;
            printf("Sender: Acknowledged frame %d\n", ack);
            frame_count--;
        }
        else
        {
            printf("Sender: NACK received, resending frame %d\n", custom_order[i - 1]);
        }

        printf("Enter -1 to end or 0 to continue: ");
        scanf("%d", &msg);
        if (msg == -1)
            break;
    }

    close(sock);
    return 0;
}
