#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

#define PORT 8080
#define WINDOW_SIZE 4
#define TOTAL_FRAMES 10

int sock = 0;
int received[TOTAL_FRAMES] = {0};
int window_start = 0;
pthread_mutex_t lock;

void *receive_frames(void *arg)
{
    int frame;
    while (window_start < TOTAL_FRAMES)
    {
        int valread = recv(sock, &frame, sizeof(frame), 0);
        if (valread > 0)
        {
            pthread_mutex_lock(&lock);
            if (frame >= window_start && frame < window_start + WINDOW_SIZE)
            {
                printf("Received frame %d\n", frame);
                received[frame] = 1;
            }
            pthread_mutex_unlock(&lock);
        }
    }
    return NULL;
}

void *send_acks(void *arg)
{
    while (window_start < TOTAL_FRAMES)
    {
        pthread_mutex_lock(&lock);
        for (int i = window_start; i < window_start + WINDOW_SIZE && i < TOTAL_FRAMES; i++)
        {
            if (received[i] == 1)
            {
                printf("Sending ACK for frame %d\n", i);
                send(sock, &i, sizeof(i), 0);
                received[i] = 2; // Acknowledged

                // Slide window forward if all frames up to window_start are acknowledged
                if (i == window_start)
                {
                    while (received[window_start] == 2 && window_start < TOTAL_FRAMES)
                    {
                        window_start++;
                    }
                }
            }
        }
        pthread_mutex_unlock(&lock);

        // Sleep for a short while before checking for unacknowledged frames
        usleep(500000); // 500 ms
    }
    return NULL;
}

int main()
{
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    pthread_t receive_thread, ack_thread;

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 3) < 0)
    {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t)&addrlen)) < 0)
    {
        perror("Accept failed");
        exit(EXIT_FAILURE);
    }

    sock = new_socket;
    pthread_mutex_init(&lock, NULL);

    // Create threads for receiving frames and sending ACKs
    pthread_create(&receive_thread, NULL, receive_frames, NULL);
    pthread_create(&ack_thread, NULL, send_acks, NULL);

    // Wait for both threads to finish
    pthread_join(receive_thread, NULL);
    pthread_join(ack_thread, NULL);

    pthread_mutex_destroy(&lock);
    close(sock);

    printf("All frames received and acknowledged.\n");
    return 0;
}