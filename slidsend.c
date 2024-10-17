#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

#define PORT 8080
#define WINDOW_SIZE 4
#define TOTAL_FRAMES 10
#define TIMEOUT 3 // Timeout in seconds

int sock = 0;
int window_start = 0;
int frame = 0;
int ack[TOTAL_FRAMES] = {0}; // Array to store ACK status of frames

pthread_mutex_t lock;

void *send_frames(void *arg)
{
    while (frame < TOTAL_FRAMES)
    {
        pthread_mutex_lock(&lock);
        int sent_frames = 0;

        // Send frames within the window
        for (int i = 0; i < WINDOW_SIZE && frame < TOTAL_FRAMES; i++)
        {
            if (ack[frame] == 0)
            { // Only send if frame not acknowledged
                printf("Sending frame %d\n", frame);
                send(sock, &frame, sizeof(frame), 0);
            }
            frame++;
            sent_frames++;
        }
        pthread_mutex_unlock(&lock);

        // Wait for a while before resending unacknowledged frames
        sleep(TIMEOUT);

        pthread_mutex_lock(&lock);
        frame = window_start; // Reset to window start for possible retransmission
        pthread_mutex_unlock(&lock);
    }
    return NULL;
}

void *receive_acks(void *arg)
{
    int valread;
    int received_ack;

    while (window_start < TOTAL_FRAMES)
    {
        valread = read(sock, &received_ack, sizeof(received_ack));
        if (valread > 0)
        {
            pthread_mutex_lock(&lock);
            printf("Received ACK for frame %d\n", received_ack);
            ack[received_ack] = 1; // Mark frame as acknowledged

            // Slide window if all frames in the current window are acknowledged
            if (received_ack == window_start)
            {
                while (ack[window_start] == 1 && window_start < TOTAL_FRAMES)
                {
                    window_start++;
                }
                frame = window_start; // Update frame to window start
            }
            pthread_mutex_unlock(&lock);
        }
    }
    return NULL;
}

int main()
{
    struct sockaddr_in serv_addr;
    pthread_t send_thread, receive_thread;

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Socket creation error \n");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0)
    {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("\nConnection Failed \n");
        return -1;
    }

    pthread_mutex_init(&lock, NULL);

    // Create threads for sending frames and receiving acknowledgments
    pthread_create(&send_thread, NULL, send_frames, NULL);
    pthread_create(&receive_thread, NULL, receive_acks, NULL);

    // Wait for both threads to finish
    pthread_join(send_thread, NULL);
    pthread_join(receive_thread, NULL);

    pthread_mutex_destroy(&lock);
    close(sock);

    printf("All frames sent and acknowledged.\n");
    return 0;
}